SHELL=/bin/sh

CXX=c++
CXXFLAGS=`xc-config --cflags` -g
LIBS=`xc-config --libs`
LDFLAGS=
MAKEDEPEND=@MAKEDEPEND@

# Rules for compiling .cc sources
.SUFFIX: .cc .o
.cc.o:
	$(CXX) $(CXXFLAGS) -c $<

SRCS = btest.cc \
	combotest.cc \
	favordlg.cc \
	ftest.cc \
	listview.cc \
	progbar.cc \
	resizer.cc \
	spinnertest.cc \
	shutter.cc \
	shutter1.cc \
	test4.cc \
	textedit.cc \
	wintest.cc \
	xdnd.cc

OBJS = $(SRCS:%.cc=%.o)

PROG = $(SRCS:%.cc=%)


all: ${PROG}

depend:
	${MAKEDEPEND} ${CXXFLAGS} ${SRCS}

install: 

clean:
	rm -f ${OBJS} $(PROG) *~ core *.bak

distclean: clean

${PROG}: ${OBJS}
	$(CXX) -o $@ $@.o $(LDFLAGS) $(LIBS) $(EXTRALIBS)
	strip $@
