include Makefile.include

DISPLAY_INCLUDE = -I./chiptune_display/
AUDIO_INCLUDE = -I./chiptune_audio/
YM_INCLUDE = -I./ymlib/
PT3_INCLUDE = -I./pt3/
VISUAL_INCLUDE = -I./visualizations/

AUDIO_LIB = ./chiptune_audio/audio.a $(LIBBCM2835)

all:	kiosk play_ym organ glados temperature falling_blocks \
	pom cpu_meter alarm_clock tengwar play_pt3
	cd conversion_tools && make

###

noteslib/notes.o:	./noteslib/notes.c
	cd noteslib && make

ymlib/ym_lib.o:	ymlib/ym_lib.c
	cd ymlib && make

ymlib/ym_play.o:	ymlib/ym_play.c
	cd ymlib && make

chiptune_display/display.a:	
	cd chiptune_display && make

chiptune_audio/audio.a:	
	cd chiptune_audio && make

visualizations/visual.a:
	cd visualizations && make

###

lib_lib.o:	lib_lib.c lib_lib.h
	$(CC) $(CFLAGS) $(AUDIO_INCLUDE) $(DISPLAY_INCLUDE) -c lib_lib.c

###

play_ym:	play_ym.o ./chiptune_audio/audio.a \
			ymlib/ym_lib.o ymlib/ym_play.o \
			chiptune_display/display.a \
			visualizations/visual.a \
			noteslib/notes.o
	$(CC) $(LFLAGS) $(LIBLHASA_LIB) -o play_ym play_ym.o \
			./chiptune_audio/audio.a \
			ymlib/ym_lib.o ymlib/ym_play.o \
			chiptune_display/display.a \
			visualizations/visual.a \
			noteslib/notes.o \
			$(LIBBCM2835)

play_ym.o:	play_ym.c ./chiptune_display/display.h ./chiptune_audio/ay-3-8910.h
	$(CC) $(CFLAGS) $(DISPLAY_INCLUDE) $(AUDIO_INCLUDE) \
		$(VISUAL_INCLUDE) $(YM_INCLUDE) \
		$(LIBLHASA_INCLUDEDIR) $(USE_LIBLHASA) -c play_ym.c

###

play_pt3:	play_pt3.o ./chiptune_audio/audio.a ./pt3/pt3_lib.o \
			chiptune_display/display.a \
			visualizations/visual.a \
			noteslib/notes.o
	$(CC) $(LFLAGS) -o play_pt3 play_pt3.o \
			./chiptune_audio/audio.a ./pt3/pt3_lib.o \
			chiptune_display/display.a \
			visualizations/visual.a \
			noteslib/notes.o \
			$(LIBBCM2835)

play_pt3.o:	play_pt3.c ./chiptune_display/display.h \
		./chiptune_audio/ay-3-8910.h ./pt3/pt3_lib.h
	$(CC) $(CFLAGS) $(DISPLAY_INCLUDE) $(AUDIO_INCLUDE) \
		$(VISUAL_INCLUDE) $(PT3_INCLUDE) \
		-c play_pt3.c



###

lyrics.o:	lyrics.c lyrics.h
	$(CC) $(CFLAGS) -c lyrics.c

glados:	glados.o chiptune_audio/audio.a \
		./ymlib/ym_lib.o ./ymlib/ym_play.o lyrics.o \
		chiptune_display/display.a lib_lib.o
	$(CC) $(LFLAGS) $(LIBLHASA_LIB) -o glados glados.o \
			lyrics.o lib_lib.o \
			./ymlib/ym_lib.o ./ymlib/ym_play.o \
			$(AUDIO_LIB) \
			chiptune_display/display.a


glados.o:	glados.c chiptune_display/display.h ymlib/ym_lib.h glados_ascii_art.h
	$(CC) $(CFLAGS) $(AUDIO_INCLUDE) $(DISPLAY_INCLUDE) $(YM_INCLUDE) $(LIBLHASA_INCLUDEDIR) $(USE_LIBLHASA) -c glados.c

###

organ:	organ.o chiptune_audio/audio.a chiptune_display/display.a noteslib/notes.o
	$(CC) $(LFLAGS) -o organ organ.o \
		chiptune_audio/audio.a \
		chiptune_display/display.a \
		noteslib/notes.o \
		$(LIBBCM2835)

organ.o:	organ.c chiptune_audio/ay-3-8910.h chiptune_display/display.h
	$(CC) $(CFLAGS) $(AUDIO_INCLUDE) $(DISPLAY_INCLUDE) -c organ.c

###

lib_temperature.o:	lib_temperature.c
	$(CC) $(CFLAGS) $(DISPLAY_INCLUDE) -c lib_temperature.c

temperature:	temperature.o lib_lib.o lib_temperature.o \
		chiptune_display/display.a chiptune_audio/audio.a
	$(CC) $(LFLAGS) -o temperature lib_lib.o lib_temperature.o temperature.o \
		chiptune_display/display.a $(AUDIO_LIB)

temperature.o:	temperature.c chiptune_display/display.h
	$(CC) $(CFLAGS) $(DISPLAY_INCLUDE) -c temperature.c


###

lib_clock.o:	lib_clock.c lib_lib.h
	$(CC) $(CFLAGS) $(AUDIO_INCLUDE) $(DISPLAY_INCLUDE) -c lib_clock.c

alarm_clock:	alarm_clock.o lib_lib.o lib_clock.o lib_pom.o \
		chiptune_display/display.a \
		chiptune_audio/audio.a
	$(CC) $(LFLAGS) -o alarm_clock alarm_clock.o lib_lib.o lib_clock.o \
		lib_pom.o \
		chiptune_display/display.a \
		chiptune_audio/audio.a \
		$(LIBBCM2835)

alarm_clock.o:	alarm_clock.c chiptune_display/display.h
	$(CC) $(CFLAGS) $(AUDIO_INCLUDE) $(DISPLAY_INCLUDE) -c alarm_clock.c

###

cpu_meter:	cpu_meter.o chiptune_display/display.a
	$(CC) $(LFLAGS) -o cpu_meter cpu_meter.o \
		chiptune_display/display.a

cpu_meter.o:	cpu_meter.c chiptune_display/display.h
	$(CC) $(CFLAGS) $(DISPLAY_INCLUDE) -c cpu_meter.c

###

lib_pom.o:	lib_pom.c lib_lib.h
	$(CC) $(CFLAGS) $(DISPLAY_INCLUDE) -c lib_pom.c

pom:	pom.o lib_lib.o lib_pom.o chiptune_display/display.a
	$(CC) $(LFLAGS) -o pom pom.o lib_lib.o lib_pom.o \
		chiptune_display/display.a $(AUDIO_LIB)

pom.o:	pom.c chiptune_display/display.h
	$(CC) $(CFLAGS) $(DISPLAY_INCLUDE) -c pom.c

###

tengwar:	tengwar.o chiptune_display/display.a
	$(CC) $(LFLAGS) -o tengwar tengwar.o chiptune_display/display.a

tengwar.o:	tengwar.c chiptune_display/display.h
	$(CC) $(CFLAGS) $(DISPLAY_INCLUDE) -c tengwar.c


###

falling_blocks:	falling_blocks.o \
		chiptune_display/display.a \
		chiptune_audio/audio.a \
		ymlib/ym_lib.o ./ymlib/ym_play.o lyrics.o
	$(CC) $(LFLAGS) $(LIBLHASA_LIB) -o falling_blocks falling_blocks.o \
		chiptune_audio/audio.a \
		ymlib/ym_lib.o ymlib/ym_play.o lyrics.o \
		chiptune_display/display.a \
		$(LIBBCM2835)

falling_blocks.o:	falling_blocks.c chiptune_display/display.h
	$(CC) $(CFLAGS) $(AUDIO_INCLUDE) $(DISPLAY_INCLUDE) $(YM_INCLUDE) -c falling_blocks.c

###


kiosk:	kiosk.o lib_lib.o lib_clock.o lib_pom.o \
		chiptune_display/display.a \
		chiptune_audio/audio.a
	$(CC) $(LFLAGS) -o kiosk kiosk.o lib_lib.o lib_clock.o lib_pom.o \
		chiptune_display/display.a \
		chiptune_audio/audio.a \
		$(LIBBCM2835)

kiosk.o:	kiosk.c chiptune_display/display.h
	$(CC) $(CFLAGS) $(AUDIO_INCLUDE) $(DISPLAY_INCLUDE) -c kiosk.c

####


clean:	
	rm -f *~ *.o play_ym organ glados \
		falling_blocks temperature pom cpu_meter \
		alarm_clock tengwar kiosk play_pt3
	cd chiptune_audio && make clean
	cd chiptune_display && make clean
	cd conversion_tools && make clean
	cd noteslib && make clean
	cd tests && make clean
	cd visualizations && make clean
	cd ymlib && make clean
