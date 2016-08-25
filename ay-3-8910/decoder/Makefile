CC = gcc
CFLAGS = -O2 -Wall

all:	ym_decode

ym_decode:	ym_decode.o
	$(CC) -o ym_decode ym_decode.o

ym_decode.o:	ym_decode.c
	$(CC) $(CFLAGS) -c ym_decode.c

clean:	
	rm -f *~ *.o ym_decode
