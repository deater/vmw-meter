include ../Makefile.include

DISPLAY_INCLUDE = -I../chiptune_display/
YM_INCLUDE = -I../ymlib/


all:	test_font test_8x16


###

ymlib/ym_lib.o:	ymlib/ym_lib.c
	cd ymlib && make

ymlib/ym_play.o:	ymlib/ym_play.c
	cd ymlib && make


###

test_font:	test_font.o ../chiptune_display/display.a
	$(CC) $(LFLAGS) -o test_font test_font.o ../chiptune_display/display.a

test_font.o:	test_font.c ../chiptune_display/display.h
	$(CC) $(CFLAGS) $(YM_INCLUDE) $(DISPLAY_INCLUDE) -c test_font.c

####

test_8x16:	test_8x16.o ../chiptune_display/display.a
	$(CC) $(LFLAGS) -o test_8x16 test_8x16.o ../chiptune_display/display.a

test_8x16.o:	test_8x16.c ../chiptune_display/display.h
	$(CC) $(CFLAGS) $(YM_INCLUDE) $(DISPLAY_INCLUDE) -c test_8x16.c


###

clean:	
	rm -f *~ *.o test_font test_8x16
