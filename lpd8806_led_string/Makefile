CC = gcc
CFLAGS = -O2 -Wall -I../include
LFLASG =

all:	run_leds

####

../lib/spi_lib.o:	../lib/spi_lib.c
	cd ../lib && make

colors.o:	colors.c colors.h
	$(CC) $(CFLAGS) -c colors.c

####

run_leds:	run_leds.o ../lib/spi_lib.o colors.o lpd8806.o \
		bargraph.o bargraph_manual.o disable.o falling.o \
		fish.o gradient.o \
		noise.o pulsar.o rainbow.o \
		scanner.o scanner_blinky.o scanner_dual.o scanner_random.o \
		stars.o two_color_scroll.o
	$(CC) $(LFLAGS) -o run_leds run_leds.o \
		../lib/spi_lib.o colors.o lpd8806.o \
		bargraph.o bargraph_manual.o disable.o falling.o \
		fish.o gradient.o\
		noise.o pulsar.o rainbow.o \
		scanner.o scanner_blinky.o scanner_dual.o scanner_random.o \
		stars.o two_color_scroll.o \
		-lm 

##########################

run_leds.o:	run_leds.c colors.h effects.h lpd8806.h
	$(CC) $(CFLAGS) -c run_leds.c

lpd8806.o:	lpd8806.c
	$(CC) $(CFLAGS) -c lpd8806.c

####

bargraph.o:	bargraph.c colors.h
	$(CC) $(CFLAGS) -c bargraph.c

bargraph_manual.o:	bargraph_manual.c colors.h
	$(CC) $(CFLAGS) -c bargraph_manual.c

disable.o:	disable.c
	$(CC) $(CFLAGS) -c disable.c

falling.o:	falling.c colors.h
	$(CC) $(CFLAGS) -c falling.c

fish.o:	fish.c colors.h
	$(CC) $(CFLAGS) -c fish.c

gradient.o:	gradient.c
	$(CC) $(CFLAGS) -c gradient.c

noise.o:	noise.c
	$(CC) $(CFLAGS) -c noise.c

pulsar.o:	pulsar.c colors.h
	$(CC) $(CFLAGS) -c pulsar.c

rainbow.o:	rainbow.c colors.h
	$(CC) $(CFLAGS) -c rainbow.c

scanner.o:	scanner.c colors.h
	$(CC) $(CFLAGS) -c scanner.c

scanner_blinky.o:	scanner_blinky.c colors.h
	$(CC) $(CFLAGS) -c scanner_blinky.c

scanner_dual.o:	scanner_dual.c
	$(CC) $(CFLAGS) -c scanner_dual.c

scanner_random.o:	scanner_random.c colors.h
	$(CC) $(CFLAGS) -c scanner_random.c

stars.o:	stars.c
	$(CC) $(CFLAGS) -c stars.c

two_color_scroll.o:	two_color_scroll.c
	$(CC) $(CFLAGS) -c two_color_scroll.c






####

clean:	
	rm -f *.o *~ run_leds 
