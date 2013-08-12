CC = gcc
CFLAGS = -Wall -O2 -g
LFLAGS =

all:	clock_demo

i2c_lib.o:	i2c_lib.c i2c_lib.h
	$(CC) $(CFLAGS) -c i2c_lib.c

####

clock_demo:	clock_demo.o i2c_lib.o
	$(CC) $(LFLASG) -o clock_demo clock_demo.o i2c_lib.o

clock_demo.o:	clock_demo.c i2c_lib.h
	$(CC) $(CFLAGS) -c clock_demo.c

####

clean:	
	rm -f *~ *.o \
		clock_demo

