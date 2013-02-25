CC = gcc
CFLAGS = -Wall -O2
LFLAGS =

all:	fill_demo spiral_demo

i2c_lib.o:	i2c_lib.c i2c_lib.h
	$(CC) $(CFLAGS) -c i2c_lib.c

####

fill_demo:	fill_demo.o i2c_lib.o
	$(CC) $(LFLASG) -o fill_demo fill_demo.o i2c_lib.o

fill_demo.o:	fill_demo.c i2c_lib.h
	$(CC) $(CFLAGS) -c fill_demo.c

####

spiral_demo:	spiral_demo.o i2c_lib.o
	$(CC) $(LFLASG) -o spiral_demo spiral_demo.o i2c_lib.o

spiral_demo.o:	spiral_demo.c i2c_lib.h
	$(CC) $(CFLAGS) -c spiral_demo.c

####

clean:	
	rm -f *~ *.o fill_demo spiral_demo

