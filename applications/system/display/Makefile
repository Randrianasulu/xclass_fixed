SHELL=/bin/sh

fvwm_includes=-I./fvwm

CXX=c++
CXXFLAGS=`xc-config --cflags` -g -g ${fvwm_includes}
LIBS=`xc-config --libs`
LDFLAGS=

# Rules for compiling .cc sources
.SUFFIX: .cc .o
.cc.o:
	$(CXX) $(CXXFLAGS) -c $<

SRCS = main.cc \
	bckgnd_tab.cc \
	scrnsav_tab.cc \
	appearance_tab.cc \
	settings_tab.cc

OBJS = $(SRCS:%.cc=%.o)

PROG = display

all: ${PROG}

${PROG}: ${OBJS}
	$(CXX) -o $@ ${OBJS} $(LDFLAGS) $(LIBS) $(EXTRALIBS)
	strip $@

install: 

clean:
	rm -f ${OBJS} $(PROG) *~ core *.bak
