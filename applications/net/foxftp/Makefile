SHELL=/bin/sh

prefix=/usr/local
exec_prefix=${prefix}

CXX=c++
CXXFLAGS=`xc-config --cflags` -g
LIBS=`xc-config --libs`
EXTRALIBS=
LDFLAGS= 
MAKEDEPEND=@MAKEDEPEND@

SRCS = OFile.cc \
	OFileSystem.cc \
	OFtp.cc \
	OFtpConfig.cc \
	OLocal.cc \
	OPackedMatrixLayout.cc \
	OXConnectDialog.cc \
	OXEditLabel.cc \
	OXFtpConfig.cc \
	OXFtpDialogs.cc \
	OXFtpManager.cc \
	OXFtpTransfer.cc \
	OXLogView.cc \
	OXProperties.cc \
	OXSiteView.cc \
	OXToolBarButton.cc \
	main.cc

OBJS = $(SRCS:.cc=.o)

PROG = foxftp

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
