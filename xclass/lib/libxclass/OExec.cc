/**************************************************************************

    This file is part of xclass.
    Copyright (C) 2001, Hector Peraza.

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

**************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

#include <xclass/OExec.h>


ODList *OExec::_childList = NULL;


//----------------------------------------------------------------------

OExec::OExec(const char *prog, char *argv[], int pipe_io, int persistent) {
  int fd0[2], fd1[2], fd2[2];

  if (!_childList) {
    _childList = new ODList("Application Children");
    if (!_childList) FatalError("OExec: could not create children list");
    signal(SIGCHLD, OExec::CatchExit);
  }

  _msgObject = NULL;

  if (pipe_io) {
    pipe(fd0);
    pipe(fd1);
    pipe(fd2);
  }

  _pid = fork();
  if (_pid == 0) {
    if (pipe_io) {
      dup2(fd0[0], fileno(stdin));
      close(fd0[1]);
      dup2(fd1[1], fileno(stdout));
      close(fd1[0]);
      dup2(fd2[1], fileno(stderr));
      close(fd2[0]);
    }
    execvp(prog, argv);
    fprintf(stderr, "OExec: failed to execute %s\n", prog);
    exit(_status = 255);
  } else {
    if (pipe_io) {
      _input_fd = fd0[1];  // we write to the application's input
      close(fd0[0]);
      _output_fd = fd1[0]; // likewise, we read fron application's output
      close(fd1[1]);
      _error_fd = fd2[0];
      close(fd2[1]);
    } else {
      _input_fd = -1;
      _output_fd = -1;
      _error_fd = -1;
    }
  }

  _childList->Add(_pid, (XPointer) this);

  _status = 0;
  _persistent = persistent;
}

OExec::~OExec() {
  if (_input_fd  >= 0) close(_input_fd);
  if (_output_fd >= 0) close(_output_fd);
  if (_error_fd  >= 0) close(_error_fd);
  if (!_persistent) {
    Kill();
    //if (_pid > 0) waitpid(_pid, &_status, WNOHANG);
  }
  _childList->Remove(_pid);
}

int OExec::Read(char *buf, int len) {
  return read(_output_fd, buf, len);
}

int OExec::ReadError(char *buf, int len) {
  return read(_error_fd, buf, len);
}

int OExec::Write(char *buf, int len) {
  return write(_input_fd, buf, len);
}

int OExec::Kill(int signal) {
  if (_pid > 0) return kill(_pid, signal);
}

int OExec::Wait(int options) {
#ifdef HAVE_WAITPID  
  return waitpid(_pid, &_status, options);
#else
  return wait4(_pid, &_status, options, NULL);
#endif
}

int OExec::IsRunning() {
  if (_pid < 0) return False;
  int retc = Wait(WNOHANG);
  if (retc == 0) return True;
  if (retc == _pid) _pid = -1;
  return False;
}

int OExec::GetExitCode() {
  if (WIFEXITED(_status)) return WEXITSTATUS(_status);
  return -1;
}

void OExec::CatchExit(int signo) {
  OXSNode *e;
  int pid, status;

  signal(SIGCHLD, OExec::CatchExit);

  pid = wait(&status);
  e = _childList->GetNode(pid);
  if (e) {
    OExec *exec = (OExec *) e->data;
    exec->_Exited(status);
  }
}

int OExec::_Exited(int status) {
  _status = status;

  OExecMessage msg(MSG_EXEC, MSG_APPEXITED, _pid);
  SendMessage(_msgObject, &msg);

  //_pid = -1;
  return 0;
}
