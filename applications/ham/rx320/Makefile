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
	ORX320.cc \
	OFreqRecord.cc \
	OXFreqDB.cc \
	OXDisplayPanel.cc \
	OXSMeter.cc \
	OXTuningKnob.cc \
	OXDialogs.cc

OBJS = $(SRCS:.cc=.o)

PROG = rx320

all: ${PROG}

${PROG}: ${OBJS}
	$(CXX) -o $@ ${OBJS} $(LDFLAGS) $(LIBS) $(EXTRALIBS)
	strip $@

install: 

clean:
	rm -f ${OBJS} $(PROG) *~ core *.bak
