SHELL=/bin/sh

CXX=c++
CXXFLAGS=`xc-config --cflags` -g
LIBS=`xc-config --libs`
LDFLAGS=

# Rules for compiling .cc sources
.SUFFIX: .cc .o
.cc.o:
	$(CXX) $(CXXFLAGS) -c $<

SRCS = main.cc \
	OXPageLBEntry.cc \
	OXGhostView.cc \
	OXDialogs.cc \
	OXTextView.cc \
	ps.cc

OBJS = $(SRCS:.cc=.o)

PROG = xcgview

all: ${PROG}

#depend:
#	${MAKEDEPEND} ${CXXFLAGS} ${SRCS}

${PROG}: ${OBJS}
	$(CXX) -o $@ ${OBJS} $(LDFLAGS) $(LIBS) $(EXTRALIBS)
	strip $@

install: 

clean:
	rm -f ${OBJS} $(PROG) *~ core *.bak
