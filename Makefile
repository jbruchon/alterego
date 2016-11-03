CC=gcc
CFLAGS=-O2 -pipe -fstrict-aliasing
CFLAGS += -Wall -Wextra -Wwrite-strings -Wcast-align -Wstrict-aliasing -pedantic -Wstrict-overflow -Wstrict-prototypes -Wpointer-arith -Wundef
CFLAGS += -Wshadow -Wfloat-equal -Wstrict-overflow=5 -Waggregate-return -Wcast-qual -Wswitch-default -Wswitch-enum -Wconversion -Wunreachable-code -Wformat=2 -Winit-self
LDFLAGS=-Wl,-O1

all: alterego manual

alterego: alterego.c

manual: alterego.1
	gzip -9 < alterego.1 > alterego.1.gz

install:
	install -D -o root -g root -m 4755 -s alterego $$DESTDIR/sbin/alterego
	install -D -o root -g root -m 0644 alterego.1.gz $$DESTDIR/usr/man/man1/alterego.1.gz

uninstall:
	rm -f $$DESTDIR/sbin/alterego $$DESTDIR/usr/man/man1/alterego.1.gz

clean:
	rm -f alterego alterego.1.gz *~ *.o

