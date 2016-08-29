CC = gcc
CFLAGS = -Wall -O2 -g -I../include
LFLAGS =

all:	sine_demo

../lib/i2c_lib.o:	../lib/i2c_lib.c ../include/i2c_lib.h
	cd ../lib && make

../lib/graphics_lib.o:	../lib/i2c_lib.c ../include/graphics_lib.h
	cd ../lib && make

####

sine_demo:	sine_demo.o ../lib/i2c_lib.o ../lib/graphics_lib.o
	$(CC) $(LFLASG) -o sine_demo sine_demo.o \
			../lib/i2c_lib.o ../lib/graphics_lib.o -lm

sine_demo.o:	sine_demo.c ../include/i2c_lib.h ../include/graphics_lib.h
	$(CC) $(CFLAGS) -c sine_demo.c


####

clean:	
	rm -f *~ *.o \
		sine_demo 

