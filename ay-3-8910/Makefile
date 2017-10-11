# Comment these out if you don't want LHA decompression
USE_LIBLHASA=-DUSE_LIBLHASA
LIBLHASA_INCLUDEDIR = -I/usr/include/liblhasa-1.0
LIBLHASA_LIB = -llhasa

CC = gcc
CFLAGS = -O2 -Wall -I../include -g
LFLAGS = -lm


DISPLAY = display.o display_bargraph.o display_keypad.o display_8x16.o display_14seg.o notes.o

all:	ym_play organ ym_to_ym5 text_to_ym text_to_ed glados test_font \
	test_8x16 temperature falling_blocks dump_ym5 dump_xm xm_to_text \
	pom cpu_meter alarm_clock tengwar

../lib/i2c_lib.o:	../lib/i2c_lib.c
	cd ../lib && make

###

xm_lib.o:	xm_lib.c xm_lib.h
	$(CC) $(CFLAGS) -c xm_lib.c

dump_xm.o:	dump_xm.c xm_lib.h
	$(CC) $(CFLAGS) -c dump_xm.c

dump_xm:	dump_xm.o xm_lib.o
	$(CC) $(LFLAGS) -o dump_xm dump_xm.o xm_lib.o


xm_to_text.o:	xm_to_text.c xm_lib.h
	$(CC) $(CFLAGS) -c xm_to_text.c

xm_to_text:	xm_to_text.o xm_lib.o
	$(CC) $(LFLAGS) -o xm_to_text xm_to_text.o xm_lib.o



###

ym_lib.o:	ym_lib.c ym_lib.h
	$(CC) $(CFLAGS) $(LIBLHASA_INCLUDEDIR) $(USE_LIBLHASA) -c ym_lib.c


ym_play:	ym_play.o ay-3-8910.o max98306.o ym_lib.o \
			$(DISPLAY) \
			../lib/i2c_lib.o
	$(CC) $(LFLAGS) $(LIBLHASA_LIB) -o ym_play ym_play.o ay-3-8910.o \
			max98306.o \
			$(DISPLAY) ym_lib.o \
			../lib/i2c_lib.o /usr/local/lib/libbcm2835.a

ym_play.o:	ym_play.c display.h ay-3-8910.h
	$(CC) $(CFLAGS) $(LIBLHASA_INCLUDEDIR) $(USE_LIBLHASA) -c ym_play.c


######

dump_ym5:	dump_ym5.o ay-3-8910.o max98306.o ym_lib.o \
			$(DISPLAY) \
			../lib/i2c_lib.o
	$(CC) $(LFLAGS) $(LIBLHASA_LIB) -o dump_ym5 dump_ym5.o ay-3-8910.o \
			max98306.o \
			$(DISPLAY) ym_lib.o \
			../lib/i2c_lib.o /usr/local/lib/libbcm2835.a

dump_ym5.o:	dump_ym5.c display.h ay-3-8910.h
	$(CC) $(CFLAGS) $(LIBLHASA_INCLUDEDIR) $(USE_LIBLHASA) -c dump_ym5.c


###

lib_lib.o:	lib_lib.c lib_lib.h
	$(CC) $(CFLAGS) -c lib_lib.c

###

lyrics.o:	lyrics.c lyrics.h
	$(CC) $(CFLAGS) -c lyrics.c

glados:	glados.o ../lib/i2c_lib.o ay-3-8910.o ym_lib.o lyrics.o \
		max98306.o \
		$(DISPLAY)
	$(CC) $(LFLAGS) $(LIBLHASA_LIB) -o glados glados.o \
			lyrics.o ay-3-8910.o ym_lib.o max98306.o \
			../lib/i2c_lib.o \
			$(DISPLAY) \
			/usr/local/lib/libbcm2835.a

glados.o:	glados.c display.h ym_lib.h glados_ascii_art.h
	$(CC) $(CFLAGS) $(LIBLHASA_INCLUDEDIR) $(USE_LIBLHASA) -c glados.c

###

organ:	organ.o ay-3-8910.o max98306.o $(DISPLAY)
	$(CC) $(LFLAGS) -o organ organ.o \
		ay-3-8910.o max98306.o \
		../lib/i2c_lib.o $(DISPLAY) /usr/local/lib/libbcm2835.a

organ.o:	organ.c ay-3-8910.h display.h
	$(CC) $(CFLAGS) -c organ.c

###

lib_temperature.o:	lib_temperature.c
	$(CC) $(CFLAGS) -c lib_temperature.c

temperature:	temperature.o lib_lib.o lib_temperature.o $(DISPLAY) \
		../lib/i2c_lib.o
	$(CC) $(LFLAGS) -o temperature lib_lib.o lib_temperature.o temperature.o \
		$(DISPLAY) \
		../lib/i2c_lib.o

temperature.o:	temperature.c display.h
	$(CC) $(CFLAGS) -c temperature.c


###

alarm_clock:	alarm_clock.o $(DISPLAY) \
		../lib/i2c_lib.o ay-3-8910.o max98306.o 
	$(CC) $(LFLAGS) -o alarm_clock alarm_clock.o \
		$(DISPLAY) \
		ay-3-8910.o max98306.o \
		/usr/local/lib/libbcm2835.a \
		../lib/i2c_lib.o


alarm_clock.o:	alarm_clock.c display.h
	$(CC) $(CFLAGS) -c alarm_clock.c

###

cpu_meter:	cpu_meter.o $(DISPLAY) \
		../lib/i2c_lib.o
	$(CC) $(LFLAGS) -o cpu_meter cpu_meter.o \
		$(DISPLAY) \
		../lib/i2c_lib.o

cpu_meter.o:	cpu_meter.c display.h
	$(CC) $(CFLAGS) -c cpu_meter.c

###

lib_pom.o:	lib_pom.c
	$(CC) $(CFLAGS) -c lib_pom.c

pom:	pom.o lib_lib.o lib_pom.o $(DISPLAY) \
		../lib/i2c_lib.o
	$(CC) $(LFLAGS) -o pom pom.o lib_lib.o lib_pom.o \
		$(DISPLAY) \
		../lib/i2c_lib.o

pom.o:	pom.c display.h
	$(CC) $(CFLAGS) -c pom.c

###

tengwar:	tengwar.o $(DISPLAY) \
		../lib/i2c_lib.o
	$(CC) $(LFLAGS) -o tengwar tengwar.o \
		$(DISPLAY) \
		../lib/i2c_lib.o

tengwar.o:	tengwar.c display.h
	$(CC) $(CFLAGS) -c tengwar.c


###

falling_blocks:	falling_blocks.o \
		$(DISPLAY) \
		../lib/i2c_lib.o ay-3-8910.o \
		ym_lib.o lyrics.o \
		max98306.o display.o display_bargraph.o
	$(CC) $(LFLAGS) $(LIBLHASA_LIB) -o falling_blocks falling_blocks.o \
		../lib/i2c_lib.o ay-3-8910.o ym_lib.o lyrics.o \
		max98306.o \
		$(DISPLAY) \
		/usr/local/lib/libbcm2835.a

falling_blocks.o:	falling_blocks.c display.h
	$(CC) $(CFLAGS) -c falling_blocks.c



###

test_font:	test_font.o $(DISPLAY) ../lib/i2c_lib.o
	$(CC) $(LFLAGS) -o test_font test_font.o $(DISPLAY) ../lib/i2c_lib.o

test_font.o:	test_font.c display.h
	$(CC) $(CFLAGS) -c test_font.c

####

test_8x16:	test_8x16.o $(DISPLAY) ../lib/i2c_lib.o
	$(CC) $(LFLAGS) -o test_8x16 test_8x16.o $(DISPLAY) \
		../lib/i2c_lib.o

test_8x16.o:	test_8x16.c display.h
	$(CC) $(CFLAGS) -c test_8x16.c


###

text_to_ym:	text_to_ym.o notes.o
	$(CC) $(LFLAGS) -o text_to_ym text_to_ym.o notes.o

text_to_ym.o:	text_to_ym.c notes.h
	$(CC) $(CFLAGS) -c text_to_ym.c

####

text_to_ed:	text_to_ed.o notes.o
	$(CC) $(LFLAGS) -o text_to_ed text_to_ed.o notes.o

text_to_ed.o:	text_to_ed.c notes.h
	$(CC) $(CFLAGS) -c text_to_ed.c


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

display_bargraph.o:	display_bargraph.c display.h
	$(CC) $(CFLAGS) -c display_bargraph.c

display_keypad.o:	display_keypad.c display.h
	$(CC) $(CFLAGS) -c display_keypad.c

display_8x16.o:	display_8x16.c display.h
	$(CC) $(CFLAGS) -c display_8x16.c

display_14seg.o:	display_14seg.c 14seg_font.h display.h font.h
	$(CC) $(CFLAGS) -c display_14seg.c



max98306.o:	max98306.c max98306.h
	$(CC) $(CFLAGS) -c max98306.c

notes.o:	notes.c notes.h
	$(CC) $(CFLAGS) -c notes.c


clean:	
	rm -f *~ *.o ym_play organ text_to_ym ym_to_ym5 glados \
		falling_blocks temperature test_font text_to_ed \
		dump_ym5 dump_xm test_8x16 xm_to_text pom cpu_meter \
		alarm_clock tengwar
