CC = gcc
CFLAGS = -Wall -O2 -g
LFLAGS = -lm
AR = ar

SDL_LIBS= `sdl-config --libs`
SDL_INCLUDE= `sdl-config --cflags`

all:	sparkle

sparkle:	sparkle.o pi-sim.a
	$(CC) -o sparkle sparkle.o pi-sim.a $(SDL_LIBS) $(LFLAGS)

sparkle.o:	sparkle.c pi-sim.h
	$(CC) $(CFLAGS) -c sparkle.c

####

pi-sim.a:	pi-sim.o apple2.o
	$(AR) crs pi-sim.a pi-sim.o apple2.o


###

pi-sim.o:	pi-sim.c pi-sim.h
	$(CC) $(CFLAGS) $(SDL_INCLUDE) -c pi-sim.c

apple2.o:	apple2.c pi-sim.h
	$(CC) $(CFLAGS) $(SDL_INCLUDE) -c apple2.c


clean:
	rm -f sparkle *~ *.o *.a

