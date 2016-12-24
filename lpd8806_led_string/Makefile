CC = gcc
CFLAGS = -O2 -Wall -I../include
LFLASG =

all:	run_leds \
	blinky_scanner blue_yellow disable dual_scanner fish \
	led_test led_scanner pulsar random red_green \
	random_scanner stars \
	manual_bargraph falling

../lib/spi_lib.o:	../lib/spi_lib.c
	cd ../lib && make

colors.o:	colors.c colors.h
	$(CC) $(CFLAGS) -c colors.c


####

run_leds:	run_leds.o ../lib/spi_lib.o colors.o lpd8806.o \
		bargraph.o rainbow.o
	$(CC) $(LFLAGS) -o run_leds run_leds.o \
		../lib/spi_lib.o colors.o lpd8806.o \
		bargraph.o rainbow.o \
		-lm 

##########################

run_leds.o:	run_leds.c colors.h
	$(CC) $(CFLAGS) -c run_leds.c

lpd8806.o:	lpd8806.c
	$(CC) $(CFLAGS) -c lpd8806.c

####

bargraph.o:	bargraph.c colors.h
	$(CC) $(CFLAGS) -c bargraph.c


####

rainbow.o:	rainbow.c colors.h
	$(CC) $(CFLAGS) -c rainbow.c


####

blinky_scanner:	blinky_scanner.o ../lib/spi_lib.o colors.o
	$(CC) $(LFLAGS) -o blinky_scanner blinky_scanner.o ../lib/spi_lib.o  colors.o

blinky_scanner.o:	blinky_scanner.c colors.h
	$(CC) $(CFLAGS) -c blinky_scanner.c



####

blue_yellow:	blue_yellow.o ../lib/spi_lib.o
	$(CC) $(LFLAGS) -o blue_yellow blue_yellow.o ../lib/spi_lib.o

blue_yellow.o:	blue_yellow.c
	$(CC) $(CFLAGS) -c blue_yellow.c


####

disable:	disable.o ../lib/spi_lib.o
	$(CC) $(LFLAGS) -o disable disable.o ../lib/spi_lib.o

disable.o:	disable.c
	$(CC) $(CFLAGS) -c disable.c


####

dual_scanner:	dual_scanner.o ../lib/spi_lib.o
	$(CC) $(LFLAGS) -o dual_scanner dual_scanner.o ../lib/spi_lib.o

dual_scanner.o:	dual_scanner.c
	$(CC) $(CFLAGS) -c dual_scanner.c


####

falling:	falling.o ../lib/spi_lib.o colors.o
	$(CC) $(LFLAGS) -o falling falling.o ../lib/spi_lib.o colors.o -lm 

falling.o:	falling.c colors.h
	$(CC) $(CFLAGS) -c falling.c



####

fish:	fish.o ../lib/spi_lib.o colors.o
	$(CC) $(LFLAGS) -o fish fish.o ../lib/spi_lib.o colors.o

fish.o:	fish.c colors.h
	$(CC) $(CFLAGS) -c fish.c


####

led_scanner:	led_scanner.o ../lib/spi_lib.o colors.o
	$(CC) $(LFLAGS) -o led_scanner led_scanner.o ../lib/spi_lib.o colors.o

led_scanner.o:	led_scanner.c colors.h
	$(CC) $(CFLAGS) -c led_scanner.c

####

led_test:	led_test.o ../lib/spi_lib.o
	$(CC) $(LFLAGS) -o led_test led_test.o ../lib/spi_lib.o

led_test.o:	led_test.c
	$(CC) $(CFLAGS) -c led_test.c

####

manual_bargraph:	manual_bargraph.o ../lib/spi_lib.o colors.o
	$(CC) $(LFLAGS) -o manual_bargraph manual_bargraph.o ../lib/spi_lib.o colors.o -lm 

manual_bargraph.o:	manual_bargraph.c colors.h
	$(CC) $(CFLAGS) -c manual_bargraph.c




####

pulsar:	pulsar.o ../lib/spi_lib.o ./colors.o
	$(CC) $(LFLAGS) -o pulsar pulsar.o ../lib/spi_lib.o ./colors.o -lm

pulsar.o:	pulsar.c colors.h
	$(CC) $(CFLAGS) -c pulsar.c


####

random:	random.o ../lib/spi_lib.o
	$(CC) $(LFLAGS) -o random random.o ../lib/spi_lib.o 

random.o:	random.c
	$(CC) $(CFLAGS) -c random.c

####

random_scanner:	random_scanner.o ../lib/spi_lib.o colors.o
	$(CC) $(LFLAGS) -o random_scanner random_scanner.o ../lib/spi_lib.o  colors.o

random_scanner.o:	random_scanner.c colors.h
	$(CC) $(CFLAGS) -c random_scanner.c


####

red_green:	red_green.o ../lib/spi_lib.o
	$(CC) $(LFLAGS) -o red_green red_green.o ../lib/spi_lib.o

red_green.o:	red_green.c
	$(CC) $(CFLAGS) -c red_green.c


####

stars:	stars.o ../lib/spi_lib.o
	$(CC) $(LFLAGS) -o stars stars.o ../lib/spi_lib.o

stars.o:	stars.c
	$(CC) $(CFLAGS) -c stars.c


####

clean:	
	rm -f *.o *~ run_leds \
		bargraph blinky_scanner blue_yellow disable dual_scanner fish \
		led_test led_scanner pulsar rainbow_standalone random red_green \
		random_scanner stars manual_bargraph falling

