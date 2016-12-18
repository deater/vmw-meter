CC = gcc
CFLAGS = -O2 -Wall -I../include
LFLAGS =

all:	nunchuck_test nunchuck_display nunchuck_tetris make_bitmap


###

nunchuck_tetris:	nunchuck_tetris.o ../lib/i2c_lib.o
	$(CC) $(LFLAGS) -o nunchuck_tetris nunchuck_tetris.o ../lib/i2c_lib.o

nunchuck_tetris.o:	nunchuck_tetris.c messages.h
	$(CC) $(CFLAGS) -c nunchuck_tetris.c


###

nunchuck_test:	nunchuck_test.o ../lib/i2c_lib.o
	$(CC) $(LFLAGS) -o nunchuck_test nunchuck_test.o ../lib/i2c_lib.o

nunchuck_test.o:	nunchuck_test.c
	$(CC) $(CFLAGS) -c nunchuck_test.c

###

nunchuck_display:	nunchuck_display.o ../lib/i2c_lib.o
	$(CC) $(LFLAGS) -o nunchuck_display nunchuck_display.o ../lib/i2c_lib.o

nunchuck_display.o:	nunchuck_display.c
	$(CC) $(CFLAGS) -c nunchuck_display.c

###

make_bitmap:	make_bitmap.o
	$(CC) $(LFLAGS) -o make_bitmap make_bitmap.o

make_bitmap.o:	make_bitmap.c
	$(CC) $(CFLAGS) -c make_bitmap.c


clean:	
	rm -f *~ *.o nunchuck_test nunchuck_display nunchuck_tetris make_bitmap


