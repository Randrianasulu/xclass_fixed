/**************************************************************************

    This file is part of rx320, a control program for the Ten-Tec RX320
    receiver. Copyright (C) 2000, 2001, Hector Peraza. Portions
    Copyright (C) 2000, A. Maitland Bottoms.

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
#include <stdlib.h>
#include <ctype.h>
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <argp.h>
#include <math.h>
#include <time.h>

#include "ORX320.h"


SFilter Filters[] = {
  { 8000, 33 }, { 6000,  0 }, { 5700,  1 }, { 5400,  2 },
  { 5100,  3 }, { 4800,  4 }, { 4500,  5 }, { 4200,  6 },
  { 3900,  7 }, { 3600,  8 }, { 3300,  9 }, { 3000, 10 },
  { 2850, 11 }, { 2700, 12 }, { 2550, 13 }, { 2400, 14 },
  { 2250, 15 }, { 2100, 16 }, { 1950, 17 }, { 1800, 18 },
  { 1650, 19 }, { 1500, 20 }, { 1350, 21 }, { 1200, 22 },
  { 1050, 23 }, {  900, 24 }, {  750, 25 }, {  675, 26 },
  {  600, 27 }, {  525, 28 }, {  450, 29 }, {  375, 30 },
  {  330, 31 }, {  300, 32 }, {    0, 32 }
};


//----------------------------------------------------------------------

ORX320::ORX320(OXClient *c, const char *dev) : OComponent() {
  _client = c;
  _msgObject = NULL;

  Mcor = 0;
  Fcor = 0.0;
  Cbfo = 0;

  _fd = -1;
  _fh = NULL;
  _ix = 0;
  _count = 0;

  _t = NULL;
  _rate = 0;

  _muted = True;
  _mode = RX320_AM;
  _spkvol = _linevol = 32;
  _filter = &Filters[0];
  _agc = RX320_AGC_MEDIUM;
  _freq = 930000;  // Hz

  if (dev) {
    OpenSerial(dev);
    Mute(True);
    Reset();
  }
}

ORX320::~ORX320() {
  CloseSerial();
  if (_t) delete _t;
}

//----------------------------------------------------------------------

int ORX320::SetSerial(const char *dev) {
  int retc;

  CloseSerial();
  if (dev) {
    retc = OpenSerial(dev);
    if (retc) {
      Mute(True);
      Reset();
    }
  }

  return retc;
}

int ORX320::HandleTimer(OTimer *t) {
  if (t == _t) {
    char scmd[2];
    scmd[0] = 'X';
    scmd[1] = 0x0D;
    SendCommand(scmd, 2);
    delete _t;
    _t = new OTimer(this, _rate);
    return True;
  }
  return False;
}

int ORX320::HandleFileEvent(OFileHandler *f, unsigned int evmask) {
  char c;

  read(_fd, &c, 1);

  if (_count == 0) {
    _ix = 1;
    if (c == 'X') {
      _count = 3;
      _inbuf[0] = c;
    } else if (c == 'Z') {
      _count = 1;
      _inbuf[0] = c;
    } else if (c == 'V') {
      _count = 6;
      _inbuf[0] = c;
    } else if (c == 'D') {
      _count = 10;
      _inbuf[0] = c;
    } else if (c == ' ') {
      _count = 0;
    } else {
      //must be some RS232 noise...
      //fprintf(stderr, "ORX320: %02x (%c)\n", (unsigned) c, c);
    }
  } else {
    _inbuf[_ix++] = c;
    if (--_count == 0) {
      c = _inbuf[0];
      if (c == 'X') {
        unsigned int val, lo, hi;
        hi = _inbuf[1]; hi &= 0xFF;
        lo = _inbuf[2]; lo &= 0xFF;
        val = (hi << 8) | lo;
        val &= 0xFFFF;
        OWidgetMessage msg(MSG_RX320, MSG_SIGNAL, val);
        SendMessage(_msgObject, &msg);
      } else if (c == 'Z') {
        OWidgetMessage msg(MSG_RX320, MSG_ERROR);
        SendMessage(_msgObject, &msg);
      } else if (strncmp(_inbuf, "VER", 3) == 0) {
        OWidgetMessage msg(MSG_RX320, MSG_VERSION, 103);
        SendMessage(_msgObject, &msg);
      } else if (strncmp(_inbuf, "DSP START", 9) == 0) {
        Reset();
        OWidgetMessage msg(MSG_RX320, MSG_POWERON, 1);
        SendMessage(_msgObject, &msg);
      }
    }
  }

  return True;
}

int ORX320::Reset() {
  SetMode(_mode);
  SetFilter(_filter->filter);
  SetVolume(RX320_SPEAKER, _spkvol);
  SetVolume(RX320_LINE, _linevol);
  SetFrequency(_freq);
  return 0;
}

int ORX320::Mute(int onoff) {
  _muted = onoff;
  SetVolume(RX320_SPEAKER, _spkvol);
  SetVolume(RX320_LINE, _linevol);
}


// MODE: AM USB LSB CW

int ORX320::SetMode(int mode) {
  char mcmd[3];

  _mode = mode;

  mcmd[0] = 'M';
  mcmd[2] = 0x0D;

  switch (mode) {
    case RX320_USB:
      Mcor = 1;
      Cbfo = 0;
      mcmd[1] = '1';
      break;

    case RX320_LSB:
      Mcor = -1;
      Cbfo = 0;
      mcmd[1] = '2';
      break;

    case RX320_CW:
      Mcor = -1;
      Cbfo = 1000;
      mcmd[1] = '3';
      break;

    default:
    case RX320_AM:
      Mcor = 0;
      mcmd[1] = '0';
      break;
  }

  SendCommand(mcmd, 3);
  return SetFrequency(_freq);
}

// FILTER: (34 of them)

int ORX320::SetFilter(int filt) {
  char fcmd[3];
  SFilter *fe;

  if (filt > 34) {
    // Allow selection by Hz as well as number
    for (fe = Filters; fe->bandwidth > 0; fe++)
      if (filt >= fe->bandwidth) break;
    filt = fe->filter;
  }
  fcmd[0] = 'W';
  fcmd[1] = filt;
  fcmd[2] = 0x0D;

  // need to set Fcor
  for (fe = Filters; filt != fe->filter; fe++);
  Fcor = ((float) fe->bandwidth / 2.0) + 200.0;

  _filter = fe;

  SendCommand(fcmd, 3);
  return SetFrequency(_freq);
}


// VOLUME: (speaker, line-out, both) 0-63
// NOTE: actually mutes the receiver if _muted is True

int ORX320::SetVolume(int output, int vol) {
  char vcmd[4];

  switch (output) {
    case RX320_SPEAKER:
      _spkvol = vol;
      vcmd[0] = 'V';
      break;

    case RX320_LINE:
      _linevol = vol;
      vcmd[0] = 'A';
      break;

    default:
    case RX320_BOTH:
      _spkvol = _linevol = vol;
      vcmd[0] = 'C';
      break;
  }
  vcmd[1] = 0;
  vcmd[3] = 0x0D;
  if ((vol >= 0) && (vol < 64)) {
    vcmd[2] = _muted ? 63 : vol;
    return SendCommand(vcmd, 4);
  }

  return 0;
}

int ORX320::GetVolume(int output) const {

  switch (output) {
    case RX320_SPEAKER:
        return _spkvol;

    case RX320_LINE:
        return _linevol;

    default:
        return 0;
  }
}


// AGC: SLOW MEDIUM FAST

int ORX320::SetAGC(int agc) {
  char agccmd[3];

  _agc = agc;

  agccmd[0] = 'G';
  switch (agc) {
    case RX320_AGC_SLOW:
      agccmd[1] = '1';
      break;

    default:
    case RX320_AGC_MEDIUM:
      agccmd[1] = '2';
      break;

    case RX320_AGC_FAST:
      agccmd[1] = '3';
      break;

  }
  agccmd[2] = 0x0D;

  return SendCommand(agccmd, 3);
}

int ORX320::SetBFO(int bfo) {
  Cbfo = bfo;
  return SetFrequency(_freq);
}

// FREQUENCY: COARSE -- FINE -- BFO

int ORX320::SetFrequency(long freq) {
  char fcmd[8];
  long AdjTfreq;		// Adjusted Tuned Frequency
  int Ctf;			// Coarse tuning factor
  int Ftf;			// Fine Tuning Factor
  int Btf;			// BFO Tuning factor

  _freq = freq;

  AdjTfreq = freq - 1250 + (int) (Mcor * (Fcor + Cbfo));
  Ctf = (int) AdjTfreq / 2500 + 18000;
  Ftf = (int) ((float) (AdjTfreq % 2500) * 5.46);
  Btf = (int) ((Fcor + (float) Cbfo + 8000.0) * 2.73);

  fcmd[0] = 'N';
  fcmd[1] = 0xFF & (Ctf >> 8);
  fcmd[2] = 0xFF & Ctf;
  fcmd[3] = 0XFF & (Ftf >> 8);
  fcmd[4] = 0xFF & Ftf;
  fcmd[5] = 0xFF & (Btf >> 8);
  fcmd[6] = 0xFF & Btf;
  fcmd[7] = 0x0D;

  return SendCommand(fcmd, 8);
}

// SIGNAL STRENGTH

int ORX320::GetSignal() {
  char scmd[2];
  unsigned char response[5];
  unsigned int hi = 0, lo = 0, i;

  if (_fd < 0) return -1;

  scmd[0] = 'X';
  scmd[1] = 0x0D;
  SendCommand(scmd, 2);

  for (i = 0; i < 4; ++i) {
    read(_fd, &response[i], 1);
  }

#if 0
  printf("[%c]: %02x %02x %02x %02x %02x\n",
         response[0], (int) response[0], (int) response[1], 
                      (int) response[2], (int) response[3],
                      (int) response[4]);
#endif

  response[4] = 0;
  if (response[0] == 'X') {
    hi = response[1];
    lo = response[2];
  }

  return (256 * hi + lo);
}

void ORX320::RequestSignal(int rate) {
  _rate = rate;
  if (_rate == 0) {
    delete _t;
    _t = NULL;
  } else if (_t == NULL) {
    _t = new OTimer(this, _rate);
  }
}

// FIRMWARE REVISION

char *ORX320::GetFirmwareVersion() {
  char vcmd[2];
  static char vers[80];

  vcmd[0] = '?';
  vcmd[1] = 0x0D;
  SendCommand(vcmd, 2);
  GetResponse(vers, 80, 0x0D);

  return vers;
}

// Send command

int ORX320::SendCommand(char *cmd, int len) {
  if (_fd < 0) return -1;
  write(_fd, cmd, len);
  return 0;
}

int ORX320::GetResponse(char *buf, int n, char term) {
  int i;

  if (_fd < 0) return 0;
  for (i = 0; i < n; i++) {
    read(_fd, &buf[i], 1);
    if (buf[i] == term) break;
  }
  buf[i] = 0;
  return i;
}

int ORX320::OpenSerial(const char *dev) {
  struct termios ts;
  int off = 0;
  int modemlines;

  _fd = open(dev, O_RDWR | O_NOCTTY | O_NONBLOCK, 0);
  if (_fd < 0) {
    perror(dev);
    return -1;
  }
  if (!isatty(_fd)) {
    // oops, need to connect to a tty
    fprintf(stderr, "%s: not a tty\n", dev);
    close(_fd);
    _fd = -1;
    return -1;
  }
  if (tcgetattr(_fd, &ts) == -1) {
    // oops, didn't get tty settings
    fprintf(stderr, "%s: failed to get tty settings\n", dev);
    close(_fd);
    _fd = -1;
    return -1;
  }
  cfmakeraw(&ts);
  ts.c_iflag = IGNBRK;	/* | IGNCR; */
  ts.c_oflag = 0;
  ts.c_cflag = CS8 | CREAD | CLOCAL;
  ts.c_lflag = 0;
  ts.c_cc[VMIN] = 1;
  ts.c_cc[VTIME] = 0;
  if (cfsetospeed(&ts, B1200) == -1) {
    // can't set output speed, see ERRNO
    perror(dev);
    close(_fd);
    _fd = -1;
    return -1;
  }
  if (cfsetispeed(&ts, B1200) == -1) {
    // can't set input speed, see ERRNO
    perror(dev);
    close(_fd);
    _fd = -1;
    return -1;
  }
  if (tcsetattr(_fd, TCSAFLUSH, &ts) == -1) {
    // can't setup serial port parms, see ERRNO
    perror(dev);
    close(_fd);
    _fd = -1;
    return -1;
  }

  // Set the line back to blocking mode after setting CLOCAL.
  if (ioctl(_fd, FIONBIO, &off) < 0) {
    // error setting blocking mode, see ERRNO
    perror(dev);
    close(_fd);
    _fd = -1;
    return -1;
  }
  modemlines = TIOCM_RTS;
  if (ioctl(_fd, TIOCMBIC, &modemlines)) {
    // can't clear RTS line, see ERRNO
    perror(dev);
    close(_fd);
    _fd = -1;
    return -1;
  }

  _fh = new OFileHandler(this, _fd, XCM_READABLE);

  return 0;
}

int ORX320::CloseSerial() {
  if (_fd >= 0) close(_fd);
  _fd = -1;
  if (_fh) delete _fh;
  _fh = NULL;
  return 0;
}
