PREFIX?=/usr/X11R6
CFLAGS?=-Os -pedantic -Wall -std=c++11

all:
	g++ $(CFLAGS) -I$(PREFIX)/include tinywm.c -L$(PREFIX)/lib -lX11 -o tinywm

clean:
	rm -f tinywm

