CC = gcc
CFLAGS = -Wall -O2 -g -I../include
LFLAGS =

all:	mpl115a2

../lib/i2c_lib.o:	../lib/i2c_lib.c ../include/i2c_lib.h
	cd ../lib && make

../lib/graphics_lib.o:	../lib/i2c_lib.c ../include/graphics_lib.h
	cd ../lib && make

####

mpl115a2:	mpl115a2.o ../lib/i2c_lib.o ../lib/graphics_lib.o
	$(CC) $(LFLASG) -o mpl115a2 mpl115a2.o \
			../lib/i2c_lib.o ../lib/graphics_lib.o -lm

mpl115a2.o:	mpl115a2.c ../include/i2c_lib.h ../include/graphics_lib.h
	$(CC) $(CFLAGS) -c mpl115a2.c


####

clean:	
	rm -f *~ *.o \
		mpl115a2 

