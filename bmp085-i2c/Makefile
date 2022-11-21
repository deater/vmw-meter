CC = gcc
CFLAGS = -Wall -O2 -g -I../include
LFLAGS =

all:	bmp085

../lib/i2c_lib.o:	../lib/i2c_lib.c ../include/i2c_lib.h
	cd ../lib && make

../lib/graphics_lib.o:	../lib/i2c_lib.c ../include/graphics_lib.h
	cd ../lib && make

####

bmp085:	bmp085.o ../lib/i2c_lib.o ../lib/graphics_lib.o
	$(CC) $(LFLASG) -o bmp085 bmp085.o \
			../lib/i2c_lib.o ../lib/graphics_lib.o -lm

bmp085.o:	bmp085.c ../include/i2c_lib.h ../include/graphics_lib.h
	$(CC) $(CFLAGS) -c bmp085.c


####

clean:	
	rm -f *~ *.o \
		bmp085 

