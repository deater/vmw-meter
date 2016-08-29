CC = gcc
CFLAGS = -Wall -O2 -g -I../include
LFLAGS =

all:	clock_demo temp_demo

../lib/i2c_lib.o:	../lib/i2c_lib.c ../include/i2c_lib.h
	cd ../lib && make

####

clock_demo:	clock_demo.o ../lib/i2c_lib.o
	$(CC) $(LFLASG) -o clock_demo clock_demo.o ../lib/i2c_lib.o

clock_demo.o:	clock_demo.c ../include/i2c_lib.h
	$(CC) $(CFLAGS) -c clock_demo.c


####

temp_demo:	temp_demo.o ../lib/i2c_lib.o
	$(CC) $(LFLASG) -o temp_demo temp_demo.o ../lib/i2c_lib.o

temp_demo.o:	temp_demo.c ../include/i2c_lib.h
	$(CC) $(CFLAGS) -c temp_demo.c


###


clean:
	rm -f *.o *~ clock_demo temp_demo

