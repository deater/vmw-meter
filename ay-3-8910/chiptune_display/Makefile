include ../Makefile.include

AY_INCLUDE = -I../chiptune_audio/
YM_INCLUDE = -I../ymlib/
I2C_INCLUDE = -I../../include

DISPLAY = display.o display_bargraph.o display_keypad.o display_8x16.o display_14seg.o
I2C = ../../lib/i2c_lib.o

all:	display.a

###

display.a:	$(DISPLAY)
	$(AR) crus display.a $(DISPLAY) $(I2C)

###

../../lib/i2c_lib.o:	../../lib/i2c_lib.c
	cd ../../lib && make

###

../noteslib/notes.o:	../noteslib/notes.c
	cd ../noteslib && make

../ymlib/ym_lib.o:	../ymlib/ym_lib.c
	cd ../ymlib && make

../ymlib/ym_play.o:	../ymlib/ym_play.c
	cd ../ymlib && make

###

display.o:	display.c display.h 
	$(CC) $(CFLAGS) $(YM_INCLUDE) $(I2C_INCLUDE) -c display.c

display_bargraph.o:	display_bargraph.c display.h
	$(CC) $(CFLAGS) $(YM_INCLUDE) $(I2C_INCLUDE) -c display_bargraph.c

display_keypad.o:	display_keypad.c display.h
	$(CC) $(CFLAGS) $(YM_INCLUDE) $(I2C_INCLUDE) -c display_keypad.c

display_8x16.o:	display_8x16.c display.h
	$(CC) $(CFLAGS) $(YM_INCLUDE) $(I2C_INCLUDE) -c display_8x16.c

display_14seg.o:	display_14seg.c 14seg_font.h display.h 
	$(CC) $(CFLAGS) $(YM_INCLUDE) $(I2C_INCLUDE) -c display_14seg.c

clean:	
	rm -f *~ *.o *.a


