CC = gcc
CFLAGS = -Wall -O3
LFLAGS =

all:	read_ps2 read_ps2_mmap

read_ps2:	read_ps2.o
	$(CC) $(LFLAGS) -o read_ps2 read_ps2.o

read_ps2.o:	read_ps2.c
	$(CC) $(CFLAGS) -c read_ps2.c


read_ps2_mmap:	read_ps2_mmap.o
	$(CC) $(LFLAGS) -o read_ps2_mmap read_ps2_mmap.o

read_ps2_mmap.o:	read_ps2_mmap.c
	$(CC) $(CFLAGS) -c read_ps2_mmap.c


clean:	
	rm -f *~ read_ps2 read_ps2_mmap *.o
