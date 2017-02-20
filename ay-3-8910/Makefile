# Comment these out if you don't want LHA decompression
USE_LIBLHASA=-DUSE_LIBLHASA
LIBLHASA_INCLUDEDIR = -I/usr/include/liblhasa-1.0
LIBLHASA_LIB = -llhasa

CC = gcc
CFLAGS = -O2 -Wall -I../include -g
LFLAGS = -lm

all:	ym_play organ ym_to_ym5 text_to_ym lyrics_play

../lib/i2c_lib.o:	../lib/i2c_lib.c
	cd ../lib && make

###

load_ym.o:	load_ym.c load_ym.h
	$(CC) $(CFLAGS) $(LIBLHASA_INCLUDEDIR) $(USE_LIBLHASA) -c load_ym.c


ym_play:	ym_play.o ay-3-8910.o display.o load_ym.o ../lib/i2c_lib.o
	$(CC) $(LFLAGS) $(LIBLHASA_LIB) -o ym_play ym_play.o ay-3-8910.o display.o \
			../lib/i2c_lib.o /usr/local/lib/libbcm2835.a

ym_play.o:	ym_play.c display.h ay-3-8910.h
	$(CC) $(CFLAGS) $(LIBLHASA_INCLUDEDIR) $(USE_LIBLHASA) -c ym_play.c

###

14seg_font.o:	14seg_font.c 14seg_font.h
	$(CC) $(CFLAGS) -c 14seg_font.c

lyrics_play:	lyrics_play.o 14seg_font.o ../lib/i2c_lib.o
	$(CC) $(LFLAGS) -o lyrics_play lyrics_play.o 14seg_font.o ../lib/i2c_lib.o

lyrics_play.o:	lyrics_play.c
	$(CC) $(CFLAGS) -c lyrics_play.c

###

organ:	organ.o ay-3-8910.o notes.o
	$(CC) $(LFLAGS) -o organ organ.o ay-3-8910.o notes.o /usr/local/lib/libbcm2835.a

organ.o:	organ.c ay-3-8910.h
	$(CC) $(CFLAGS) -c organ.c

###

text_to_ym:	text_to_ym.o notes.o
	$(CC) $(LFLAGS) -o text_to_ym text_to_ym.o notes.o

text_to_ym.o:	text_to_ym.c notes.h
	$(CC) $(CFLAGS) -c text_to_ym.c

###

ym_to_ym5:	ym_to_ym5.o
	$(CC) $(LFLAGS) -o ym_to_ym5 ym_to_ym5.o

ym_to_ym5.o:	ym_to_ym5.c
	$(CC) $(CFLAGS) -c ym_to_ym5.c

###

ay-3-8910.o:	ay-3-8910.c ay-3-8910.h
	$(CC) $(CFLAGS) -c ay-3-8910.c

display.o:	display.c display.h font.h
	$(CC) $(CFLAGS) -c display.c

notes.o:	notes.c notes.h
	$(CC) $(CFLAGS) -c notes.c


clean:	
	rm -f *~ *.o ym_play organ text_to_ym ym_to_ym5 lyrics_play









