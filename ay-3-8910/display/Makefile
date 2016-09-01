CC = gcc
CFLAGS = -Wall -O2 -g -I../../include
LFLAGS =

all:	bar_demo

../../lib/i2c_lib.o:	../../lib/i2c_lib.c ../../include/i2c_lib.h
	cd ../../lib && make

../../lib/graphics_lib.o:	../../lib/graphics_lib.c ../../include/graphics_lib.h
	cd ../../lib && make


####

bar_demo:	bar_demo.o ../../lib/i2c_lib.o
	$(CC) $(LFLASG) -o bar_demo bar_demo.o ../../lib/i2c_lib.o

bar_demo.o:	bar_demo.c ../../include/i2c_lib.h
	$(CC) $(CFLAGS) -c bar_demo.c

####

clean:	
	rm -f *~ *.o \
		bar_demo 

