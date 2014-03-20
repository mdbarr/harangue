PROGNAME = harangue
LIBS = -linfostd -lvcsi -lm -lpthread -ldl
OBJECTS = harangue.o engine.o display.o unicode.o timer.o irc.o
HEADERS = harangue.ht imer.h engine.h display.h colors.h unicode.h irc.h
CFLAGS = -O2 -g -Wall -Wno-pointer-sign -Wno-switch
BINDIR = /usr/local/bin
DATADIR = /usr/local/share/harangue

all: $(PROGNAME)

$(PROGNAME): $(OBJECTS)
	gcc -rdynamic -o $(PROGNAME) $(OBJECTS) $(LIBS)

$(OBJECTS): Makefile

.c.o: $(HEADERS)
	gcc -c $(CFLAGS) $(INCLUDES) -o $@ $< 

clean:
	rm -fv *.o *~ $(PROGNAME)

install: $(PROGNAME)
	if test ! -d ${BINDIR}; then mkdir ${BINDIR}; fi
	/usr/bin/install -c -m 755 $(PROGNAME) $(BINDIR)/$(PROGNAME)

test: $(PROGNAME)
	./harangue irc.scm
