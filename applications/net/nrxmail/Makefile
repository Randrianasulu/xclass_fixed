SHELL=/bin/sh

prefix=/usr/local
exec_prefix=${prefix}

CXX=c++
CXXFLAGS=`xc-config --cflags` -g 
LIBS=`xc-config --libs`
EXTRALIBS=
LDFLAGS= 
MAKEDEPEND=@MAKEDEPEND@

SRCS =	OFolder.cc \
	OMail.cc \
	OMailList.cc \
	OMimeAtt.cc \
	OSession.cc \
	OSmtp.cc \
	OText.cc \
	OXBodyView.cc \
	OXComposer.cc \
	OXFolderDelete.cc \
	OXFolderProp.cc \
	OXMailList.cc \
	OXMoveMail.cc \
	OXPreferences.cc \
	OXPrintBox.cc \
	OXSearchBox.cc \
	OXViewSrc.cc \
	main.cc

OBJS = $(SRCS:.cc=.o)

PROG = nrxmail

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
