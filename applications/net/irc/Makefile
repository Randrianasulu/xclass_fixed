SHELL=/bin/sh

prefix=/usr/local
exec_prefix=${prefix}

CXX=c++
CXXFLAGS=`xc-config --cflags` -g
LIBS=`xc-config --libs`
EXTRALIBS=
LDFLAGS= 
MAKEDEPEND=@MAKEDEPEND@

SRCS = OIrc.cc \
	OLineDoc.cc \
	OTcp.cc \
	OTextDoc.cc \
	OViewDoc.cc \
	OXChannel.cc \
	OXChannelDialog.cc \
	OXChannelEditor.cc \
	OXChannelList.cc \
	OXChatChannel.cc \
	OXCommandDlg.cc \
	OXConfirmDlg.cc \
	OXDCCChannel.cc \
	OXDCCFile.cc \
	OXIrc.cc \
	OXMsgChannel.cc \
	OXNameList.cc \
	OXPasswdDlg.cc \
	OXPreferences.cc \
	OXServerDlg.cc \
	OXServerTree.cc \
	OXViewDoc.cc \
	OXViewLogFile.cc \
	main.cc

OBJS = $(SRCS:.cc=.o)

PROG = foxirc

.SUFFIX: .cc .o
.cc.o:
	$(CXX) $(CXXFLAGS) -c $<

all: ${PROG} mirc2ansi

depend:
	${MAKEDEPEND} ${CXXFLAGS} ${SRCS}

install:

clean:
	${RM} ${OBJS} $(PROG) mirc2ansi *~ core *.bak

distclean: clean

${PROG}: ${OBJS}
	$(CXX) -o $@ ${OBJS} $(LDFLAGS) $(LIBS) $(EXTRALIBS)
	strip $@

mirc2ansi: mirc2ansi.c
	$(CC) -o $@ $<
