CC = gcc
CFLAGS = -Wall -O2 -g -I../include
LFLAGS =

all:	sounder_demo keyboard_demo

###

../lib/i2c_lib.o:	../lib/i2c_lib.c ../include/i2c_lib.h
	cd ../lib && make

../lib/graphics_lib.o:	../lib/graphics_lib.c ../include/graphics_lib.h
	cd ../lib && make

####

sounder_demo:	sounder_demo.o ../lib/i2c_lib.o
	$(CC) $(LFLASG) -o sounder_demo sounder_demo.o ../lib/i2c_lib.o

sounder_demo.o:	sounder_demo.c ../include/i2c_lib.h
	$(CC) $(CFLAGS) -c sounder_demo.c


####

keyboard_demo:	keyboard_demo.o ../lib/i2c_lib.o
	$(CC) $(LFLASG) -o keyboard_demo keyboard_demo.o ../lib/i2c_lib.o

keyboard_demo.o:	keyboard_demo.c ../include/i2c_lib.h
	$(CC) $(CFLAGS) -c keyboard_demo.c


clean:
	rm -f *~ *.o sounder_demo keyboard_demo
