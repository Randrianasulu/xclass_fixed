SHELL=/bin/sh

prefix=/usr/local
exec_prefix=${prefix}

CXX=c++
CXXFLAGS=`xc-config --cflags` -g 
LIBS=`xc-config --libs`
EXTRALIBS=
LDFLAGS= 
MAKEDEPEND=@MAKEDEPEND@

SRCS = main.cc \
	OGC.cc \
	OIrc.cc \
	OLineDoc.cc \
	OTcp.cc \
	OTextDoc.cc \
	OViewDoc.cc \
	OXIrc.cc \
	OXChannel.cc \
	OXChatChannel.cc \
	OXMsgChannel.cc \
	OXDCCChannel.cc \
	OXChannelDialog.cc \
	OXChannelEditor.cc \
	OXNameList.cc \
	OXConfirmDlg.cc \
	OXServerDlg.cc \
	OXPreferences.cc \
	OXDCCFile.cc \
	OXTextView.cc \
	OXColorSelect.cc \
	OXViewDoc.cc \
	OXSDList.cc

OBJS = $(SRCS:.cc=.o)

PROG = foxirc

.SUFFIX: .cc .o
.cc.o:
	$(CXX) $(CXXFLAGS) -c $<

all: ${PROG}

depend:
	${MAKEDEPEND} ${CXXFLAGS} ${SRCS}

install:

clean:
	${RM} ${OBJS} $(PROG) *~ core *.bak

distclean: clean

${PROG}: ${OBJS}
	$(CXX) -o $@ ${OBJS} $(LDFLAGS) $(LIBS) $(EXTRALIBS)
	strip $@
