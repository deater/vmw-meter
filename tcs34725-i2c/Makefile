CC = gcc
CFLAGS = -Wall -O2 -g -I../include
LFLAGS =

all:	tcs34725

../lib/i2c_lib.o:	../lib/i2c_lib.c ../include/i2c_lib.h
	cd ../lib && make

../lib/graphics_lib.o:	../lib/i2c_lib.c ../include/graphics_lib.h
	cd ../lib && make

####

tcs34725:	tcs34725.o ../lib/i2c_lib.o ../lib/graphics_lib.o
	$(CC) $(LFLASG) -o tcs34725 tcs34725.o \
			../lib/i2c_lib.o ../lib/graphics_lib.o

tcs34725.o:	tcs34725.c tcs34725.h ../include/i2c_lib.h ../include/graphics_lib.h
	$(CC) $(CFLAGS) -c tcs34725.c


####

clean:	
	rm -f *~ *.o \
		tcs34725 

