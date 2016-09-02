CC = gcc
CFLAGS = -O2 -Wall -I../../include -g

all:	ym_play

ym_play:	ym_play.o ay-3-8910.o
	$(CC) -o ym_play ym_play.o ay-3-8910.o /usr/local/lib/libbcm2835.a

ym_play.o:	ym_play.c
	$(CC) $(CFLAGS) -c ym_play.c

ay-3-8910.o:	ay-3-8910.c ay-3-8910.h
	$(CC) $(CFLAGS) -c ay-3-8910.c

clean:	
	rm -f *~ *.o ym_play
