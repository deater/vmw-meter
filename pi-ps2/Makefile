CC = gcc
CFLAGS = -Wall -O2
LFLAGS =

all:	read_ps2

read_ps2:	read_ps2.o
	$(CC) $(LFLAGS) -o read_ps2 read_ps2.o

read_ps2.o:	read_ps2.c
	$(CC) $(CFLAGS) -c read_ps2.c

clean:	
	rm -f *~ read_ps2 *.o
