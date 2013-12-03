CC = gcc
CFLAGS = -O2 -Wall -I../lib
LFLAGS =

all:	nunchuck_test nunchuck_display nunchuck_tetris


###

nunchuck_tetris:	nunchuck_tetris.o ../lib/i2c_lib.o
	$(CC) $(LFLAGS) -o nunchuck_tetris nunchuck_tetris.o ../lib/i2c_lib.o

nunchuck_tetris.o:	nunchuck_tetris.c
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


clean:	
	rm -f *~ *.o nunchuck_test nunchuck_display nunchuck_tetris


