CC = gcc
CFLAGS = -O2 -Wall -I../../include

all:	ym_play

ym_play:	ym_play.o
	$(CC) -o ym_play ym_play.o /usr/local/lib/libbcm2835.a

ym_play.o:	ym_play.c
	$(CC) $(CFLAGS) -c ym_play.c

clean:	
	rm -f *~ *.o ym_play
