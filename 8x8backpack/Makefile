CC = gcc
CFLAGS = -Wall -O2
LFLAGS =

all:	fill_demo

i2c_lib.o:	i2c_lib.c i2c_lib.h
	$(CC) $(CFLAGS) -c i2c_lib.c

fill_demo:	fill_demo.o i2c_lib.o
	$(CC) $(LFLASG) -o fill_demo fill_demo.o i2c_lib.o

fill_demo.o:	fill_demo.c i2c_lib.h
	$(CC) $(CFLAGS) -c fill_demo.c

clean:	
	rm -f *~ *.o fill_demo

