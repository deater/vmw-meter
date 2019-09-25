CC = gcc
CFLAGS = -Wall -O2 -g
LFLAGS = -lm
AR = ar

SDL_LIBS= `sdl-config --libs`
SDL_INCLUDE= `sdl-config --cflags`

all:	sparkle pi_opener


####

sparkle:	sparkle.o pi-sim.a
	$(CC) -o sparkle sparkle.o pi-sim.a $(SDL_LIBS) $(LFLAGS)

sparkle.o:	sparkle.c pi-sim.h
	$(CC) $(CFLAGS) -c sparkle.c

####

pi_opener:	pi_opener.o pcx_load.o print_string.o default_font.o \
		pi_boot.o pi-sim.a
	$(CC) -o pi_opener pi_opener.o pcx_load.o \
		print_string.o default_font.o pi_boot.o \
		pi-sim.a $(SDL_LIBS) $(LFLAGS)

pi_opener.o:	pi_opener.c pi-sim.h
	$(CC) $(CFLAGS) -c pi_opener.c


####

pi_boot.o:	pi_boot.pcx
	ld -r -b binary pi_boot.pcx -o pi_boot.o

####

pi-sim.a:	pi-sim.o apple2.o
	$(AR) crs pi-sim.a pi-sim.o apple2.o


###

pi-sim.o:	pi-sim.c pi-sim.h
	$(CC) $(CFLAGS) $(SDL_INCLUDE) -c pi-sim.c

apple2.o:	apple2.c pi-sim.h
	$(CC) $(CFLAGS) $(SDL_INCLUDE) -c apple2.c

pcx_load.o:	pcx_load.c pcx_load.h
	$(CC) $(CFLAGS) -c pcx_load.c

print_string.o:	print_string.c print_string.h default_font.h
	$(CC) $(CFLAGS) -c print_string.c

default_font.o:	default_font.c default_font.h
	$(CC) $(CFLAGS) -c default_font.c

clean:
	rm -f sparkle pi_opener *~ *.o *.a

