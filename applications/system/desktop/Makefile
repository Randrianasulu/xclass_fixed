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
	URL.cc \
	ORecycledFiles.cc \
	ODesktopLayout.cc \
	OXDesktopIcon.cc \
	OXDesktopContainer.cc \
	OXDesktopRoot.cc

OBJS = $(SRCS:.cc=.o)

PROG = desktop

all: ${PROG}

${PROG}: ${OBJS}
	$(CXX) -o $@ ${OBJS} $(LDFLAGS) $(LIBS) $(EXTRALIBS)
	strip $@

install: 

clean:
	rm -f ${OBJS} $(PROG) *~ core *.bak
