include ../Makefile.include

YM_INCLUDE = -I../ymlib/
DISPLAY_INCLUDE = -I../chiptune_display/

all:	visual.a

###

visual.a:	visualization_core.o display_8x16.o
	$(AR) crus visual.a visualization_core.o display_8x16.o

###

../../lib/i2c_lib.o:	../../lib/i2c_lib.c
	cd ../../lib && make

###

../noteslib/notes.o:	../noteslib/notes.c
	cd ../noteslib && make

###

display_8x16.o:	display_8x16.c ../chiptune_display/display.h
	$(CC) $(CFLAGS) $(YM_INCLUDE) $(DISPLAY_INCLUDE) -c display_8x16.c

visualization_core.o:	visualization_core.c visualizations.h
	$(CC) $(CFLAGS) $(YM_INCLUDE) $(DISPLAY_INCLUDE) -c visualization_core.c

clean:	
	rm -f *~ *.o *.a


