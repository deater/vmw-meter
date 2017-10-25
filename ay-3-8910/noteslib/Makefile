CC = gcc
CFLAGS = -O2 -Wall -I../include -g
LFLAGS = -lm

all:	notes.o

notes.o:	notes.c notes.h
	$(CC) $(CFLAGS) -c notes.c


clean:	
	rm -f *~ *.o
