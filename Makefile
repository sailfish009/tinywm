PREFIX?=/usr/X11R6
CFLAGS?=-Os -pedantic -Wall -std=c++11 -Wno-unused-result
LDFLAGS?=-L$(PREFIX)/lib -lX11

all:
	g++ $(CFLAGS) win.c $(LDFLAGS) -o win
	g++ $(CFLAGS) -I$(PREFIX)/include tinywm.c -L$(PREFIX)/lib -lX11 -lXtst -lpthread -o tinywm

clean:
	rm -f tinywm win

