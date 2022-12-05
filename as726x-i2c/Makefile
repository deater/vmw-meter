CC = gcc
CFLAGS = -Wall -O2 -g -I../include
LFLAGS =

all:	as7262

../lib/i2c_lib.o:	../lib/i2c_lib.c ../include/i2c_lib.h
	cd ../lib && make

../lib/graphics_lib.o:	../lib/i2c_lib.c ../include/graphics_lib.h
	cd ../lib && make

####

as7262:	as7262.o ../lib/i2c_lib.o ../lib/graphics_lib.o
	$(CC) $(LFLASG) -o as7262 as7262.o \
			../lib/i2c_lib.o ../lib/graphics_lib.o

as7262.o:	as7262.c as7262.h ../include/i2c_lib.h ../include/graphics_lib.h
	$(CC) $(CFLAGS) -c as7262.c


####

clean:	
	rm -f *~ *.o \
		as7262 

