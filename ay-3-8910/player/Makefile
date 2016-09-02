CC = gcc
CFLAGS = -O2 -Wall -I../../include -g

all:	ym_play

../../lib/i2c_lib.o:	../../lib/i2c_lib.c
	cd ../../lib && make

ym_play:	ym_play.o ay-3-8910.o display.o ../../lib/i2c_lib.o
	$(CC) -o ym_play ym_play.o ay-3-8910.o display.o \
			../../lib/i2c_lib.o /usr/local/lib/libbcm2835.a

ym_play.o:	ym_play.c
	$(CC) $(CFLAGS) -c ym_play.c

ay-3-8910.o:	ay-3-8910.c ay-3-8910.h
	$(CC) $(CFLAGS) -c ay-3-8910.c

display.o:	display.c display.h
	$(CC) $(CFLAGS) -c display.c

clean:	
	rm -f *~ *.o ym_play
