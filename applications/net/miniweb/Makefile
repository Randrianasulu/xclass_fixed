SHELL=/bin/sh

CXX=c++
CXXFLAGS=`xc-config --cflags` -g -I. -I..
LIBS=`xc-config --libs`
LDFLAGS=

# Rules for compiling .cc sources
.SUFFIX: .cc .o
.cc.o:
	$(CXX) $(CXXFLAGS) -c $<

SRCS = main.cc \
	OXWebHtml.cc

OBJS = $(SRCS:.cc=.o)

PROG = miniweb

all: ${PROG}

${PROG}: ${OBJS}
	$(CXX) -o $@ ${OBJS} $(LDFLAGS) $(LIBS) $(EXTRALIBS)
	strip $@

install: 

clean:
	rm -f ${OBJS} $(PROG) *~ core *.bak
