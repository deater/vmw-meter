CC = gcc
CFLAGS = -Wall -O2
LFLAGS =

all:	fill_demo

fill_demo:	fill_demo.o
	$(CC) $(LFLASG) -o fill_demo fill_demo.o

fill_demo.o:	fill_demo.c
	$(CC) $(CFLAGS) -c fill_demo.c

clean:	
	rm -f *~ *.o fill_demo

