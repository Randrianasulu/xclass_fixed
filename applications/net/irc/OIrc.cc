#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include "OIrcMessage.h"
#include "IRCcodes.h"
#include "OIrc.h"

//----------------------------------------------------------------------

OIrc::OIrc() : OComponent() {
  _tcp = new OTcp();
  _tcp->Associate(this);
}

OIrc::~OIrc() {
  delete _tcp;
}

int OIrc::ProcessMessage(OMessage *msg) {
  char char1[TCP_BUFFER_LENGTH], char2[TCP_BUFFER_LENGTH],
       char3[TCP_BUFFER_LENGTH], char4[TCP_BUFFER_LENGTH],
       char5[TCP_BUFFER_LENGTH], char6[TCP_BUFFER_LENGTH];
  int  int1, int2, int3;

  switch(msg->type) {
    case INCOMING_TCP_MSG:
      {
        OTcpMessage *tcpmsg = (OTcpMessage*) msg;
        char *tcp_message = (char*)(tcpmsg->string1->GetString());

	if (strlen(tcp_message) == 0) break;

        if (sscanf(tcp_message, "PING :%s", char1) == 1) {
          sprintf(char6, "PONG :%s\n", char1);
          OTcpMessage message(OUTGOING_TCP_MSG, 0, 0, 0, 0, 0, char6);
          SendMessage(_tcp, &message);

        } else if (!strncmp(tcp_message, "ERROR", 5)) {
          OIrcMessage message(INCOMING_IRC_MSG, ERROR, 0, 0, 0, 0,
                              tcp_message+7);
          SendMessage(_msgObject, &message);

        } else if (sscanf(tcp_message, ":%s %i %s @ %s :%n",
                                      char1, &int1, char2, char3, &int2)==4) {
          OIrcMessage message(INCOMING_IRC_MSG, SPECIAL, int1, 0, 0, 0,
	                      char1, char2, char3, tcp_message+int2);
          SendMessage(_msgObject, &message);

        } else if (sscanf(tcp_message, ":%s %i %s = %s :%n",
                                   char1, &int1, char2, char3, &int2)==4) {
          OIrcMessage message(INCOMING_IRC_MSG, SPECIAL, int1, 0, 0, 0,
	                      char1, char2, char3, tcp_message+int2);
          SendMessage(_msgObject, &message);

        } else if (sscanf(tcp_message, ":%s %i %s %n",
		                   char1, &int1, char2, &int2)==3) {
          if (tcp_message[int2] == ':') ++int2;
          OIrcMessage message(INCOMING_IRC_MSG, SPECIAL, int1, 0, 0, 0,
	                      char1, char2, "", tcp_message+int2);
          SendMessage(_msgObject, &message);

        } else if (sscanf(tcp_message, ":%[^!]!%s %n",
                                   char1, char2, &int1)==2) {
          if (sscanf(tcp_message+int1, "PRIVMSG %s :%n", char3, &int2)==1) {
            OIrcMessage message(INCOMING_IRC_MSG, PRIVMSG, 0, 0, 0, 0,
	                        char3, char1, tcp_message+int1+int2);
            SendMessage(_msgObject, &message);
          } else if (sscanf(tcp_message+int1, "NOTICE %s :%n",
	                                      char3, &int2)==1) {
            OIrcMessage message(INCOMING_IRC_MSG, NOTICE, 0, 0, 0, 0,
	                        char3, char1, tcp_message+int1+int2);
            SendMessage(_msgObject, &message);
          } else if (sscanf(tcp_message+int1, "JOIN :%s", char3)==1) {
            OIrcMessage message(INCOMING_IRC_MSG, JOIN, 0, 0, 0, 0,
	                        char3, char1, char2);
            SendMessage(_msgObject, &message);
          } else if (sscanf(tcp_message+int1,"PART %s%n", char3, &int2)==1) {
            if (tcp_message[int1+int2] == ' ') ++int2;
            if (tcp_message[int1+int2] == ':') ++int2;
            OIrcMessage message(INCOMING_IRC_MSG, LEAVE, 0, 0, 0, 0,
	                        char3, char1, tcp_message+int1+int2);
            SendMessage(_msgObject, &message);
          } else if (sscanf(tcp_message+int1,"NICK :%s", char3)==1) {
            OIrcMessage message(INCOMING_IRC_MSG, NICK, 0, 0, 0, 0,
	                        char3, char1);
            SendMessage(_msgObject, &message);
	  } else if (sscanf(tcp_message+int1,"KICK %s %s :%n",
	                                     char3, char4, &int2)==2) {
            OIrcMessage message(INCOMING_IRC_MSG, KICK, 0, 0, 0, 0,
                                char3, char1, char4, tcp_message+int1+int2);
            ProcessMessage(&message);
          } else if (sscanf(tcp_message+int1,"TOPIC %s :%n",
                                             char3, &int2)==1) {
            OIrcMessage message(INCOMING_IRC_MSG, TOPIC_SET, 0, 0, 0, 0,
                                char3, char1, tcp_message+int1+int2);
            ProcessMessage(&message);
          } else if (sscanf(tcp_message+int1,"MODE %s %n",
                                             char3, &int2)==1) {
            if (tcp_message[int1+int2] == ':') ++int2;
            OIrcMessage message(INCOMING_IRC_MSG, MODE, 0, 0, 0, 0,
                                char3, char1, tcp_message+int1+int2);
            ProcessMessage(&message);
          } else if (!strncmp(tcp_message+int1,"QUIT :", 6)) {
            OIrcMessage message(INCOMING_IRC_MSG, QUIT, 0, 0, 0, 0,
	                        char1, tcp_message+int1+6);
            SendMessage(_msgObject, &message);
          } else if (!strncmp(tcp_message+int1,"WALLOPS :", 9)) {
            OIrcMessage message(INCOMING_IRC_MSG, WALLOPS, 0, 0, 0, 0,
	                        char1, tcp_message+int1+9);
            SendMessage(_msgObject, &message);
          } else if (sscanf(tcp_message+int1,"INVITE %s :%s",
	                                     char2, char3)==2) {
            OIrcMessage message(INCOMING_IRC_MSG, INVITE, 0, 0, 0, 0,
	                        char1, char2,char3);
            SendMessage(_msgObject, &message);
          } else {
            OIrcMessage message(INCOMING_IRC_MSG, UNKNOWN, 0, 0, 0, 0,
	                        tcp_message);
            SendMessage(_msgObject, &message);
          }

        } else if (sscanf(tcp_message,":%s MODE %s %n",
                                   char1, char3, &int1)==2) {
          if (tcp_message[int1] == ':') ++int1;
          OIrcMessage message(INCOMING_IRC_MSG, MODE, 0, 0, 0, 0,
                              char3, char1, tcp_message+int1);
          ProcessMessage(&message);

        } else if (sscanf(tcp_message,":%s NOTICE %s :%n",
                                   char1, char3, &int1)==2) {
          if (tcp_message[int1] == ':') ++int1;
          // char3 is not necessarily always AUTH!!!
          // but this processing should be OK for now anyway...
          OIrcMessage message(INCOMING_IRC_MSG, AUTH, 0, 0, 0, 0,
                              char3, char1, tcp_message+int1);
          SendMessage(_msgObject, &message);

        } else if (sscanf(tcp_message,"NOTICE %s :%n",
                                   char1, &int1)==1) {
          if (tcp_message[int1] == ':') ++int1;
          // this should be equally OK for now...
          OIrcMessage message(INCOMING_IRC_MSG, AUTH, 0, 0, 0, 0,
                              char1, char1, tcp_message+int1);
          SendMessage(_msgObject, &message);

        } else {
          OIrcMessage message(INCOMING_IRC_MSG, UNKNOWN, 0, 0, 0, 0,
	                      tcp_message);
          SendMessage(_msgObject, &message);
        }
      }
      break;

    case OUTGOING_TCP_MSG:
      printf("!!!!!!    TCP message sent directly from OXIrc to OTcp.\n");
      SendMessage(_tcp, msg);
      break;

    case INCOMING_IRC_MSG:
      printf("!!!!!!    Incoming IRC message sent to OIrc.\n");
      SendMessage(_msgObject, msg);
      break;

    case OUTGOING_IRC_MSG:
      {
        OIrcMessage *ircmsg = (OIrcMessage *) msg;

        switch(msg->action) {
          case NICK:
            sprintf(char1, "NICK %s\n",
		    ircmsg->string1->GetString());
	    {
              OTcpMessage message(OUTGOING_TCP_MSG, 0, 0, 0, 0, 0, char1);
              SendMessage(_tcp, &message);
	    }
            break;

          case USER:
            sprintf(char1, "USER %s %s %s :%s\n",
	            ircmsg->string1->GetString(),  // getenv("USER")
	            ircmsg->string2->GetString(),  // getenv("HOSTNAME")
	            ircmsg->string3->GetString(),  // server
		    ircmsg->string4->GetString()); // getenv("LOGNAME")
	    {
              OTcpMessage message(OUTGOING_TCP_MSG, 0, 0, 0, 0, 0, char1);
              SendMessage(_tcp, &message);
	    }
            break;

          case JOIN:
            sprintf(char1, "JOIN %s\nMODE %s\n", 
	            ircmsg->string1->GetString(), 
		    ircmsg->string1->GetString());
	    {
              OTcpMessage message(OUTGOING_TCP_MSG, 0, 0, 0, 0, 0, char1);
              SendMessage(_tcp, &message);
	    }
	    break;

          case WHO:
            sprintf(char1, "NAMES %s\n", ircmsg->string1->GetString());
	    {
              OTcpMessage message(OUTGOING_TCP_MSG, 0, 0, 0, 0, 0, char1);
              SendMessage(_tcp, &message);
	    }
	    break;

          default:
	    break;
	}
      }
      break;

    case BROKEN_PIPE:
      printf("Remote end closed connection.\n");
      SendMessage(_msgObject, msg);
      break;
  }

  return True;
}
