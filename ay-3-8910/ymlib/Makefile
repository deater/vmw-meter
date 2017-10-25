include ../Makefile.include

AY_INCLUDE = -I../chiptune_board/

CC = gcc
CFLAGS = -O2 -Wall -I../include -g
LFLAGS = -lm


DISPLAY = display.o display_bargraph.o display_keypad.o display_8x16.o display_14seg.o notes.o

all:	ym_lib.o ym_play.o


../lib/i2c_lib.o:	../lib/i2c_lib.c
	cd ../lib && make

###

ym_lib.o:	ym_lib.c ym_lib.h ../chiptune_board/ay-3-8910.h
	$(CC) $(CFLAGS) $(AY_INCLUDE) $(LIBLHASA_INCLUDEDIR) $(USE_LIBLHASA) -c ym_lib.c

ym_play.o:	ym_play.c ym_lib.h ../chiptune_board/ay-3-8910.h
	$(CC) $(CFLAGS) $(AY_INCLUDE) -c ym_play.c


###

clean:	
	rm -f *~ *.o 

