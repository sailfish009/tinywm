PREFIX?=/usr/X11R6
CFLAGS?=-Os -pedantic -Wall -std=c++2a -Wno-unused-result
LDFLAGS?=-L$(PREFIX)/lib -lX11

all:
	clang++ $(CFLAGS) win.cpp $(LDFLAGS) -o win
	clang++ $(CFLAGS) -I$(PREFIX)/include tinywm.cpp -L$(PREFIX)/lib -lX11 -lXtst -lpthread -o tinywm

clean:
	rm -f tinywm win

