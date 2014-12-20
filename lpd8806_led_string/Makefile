CC = gcc
CFLAGS = -O2 -Wall -I../lib
LFLASG =

all:	blue_yellow disable dual_scanner \
	led_test led_scanner pulsar random red_green rainbow

../lib/spi_lib.o:	../lib/spi_lib.c
	cd ../lib && make

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

led_scanner:	led_scanner.o ../lib/spi_lib.o
	$(CC) $(LFLAGS) -o led_scanner led_scanner.o ../lib/spi_lib.o

led_scanner.o:	led_scanner.c
	$(CC) $(CFLAGS) -c led_scanner.c

####

led_test:	led_test.o ../lib/spi_lib.o
	$(CC) $(LFLAGS) -o led_test led_test.o ../lib/spi_lib.o

led_test.o:	led_test.c
	$(CC) $(CFLAGS) -c led_test.c


####

pulsar:	pulsar.o ../lib/spi_lib.o
	$(CC) $(LFLAGS) -o pulsar pulsar.o ../lib/spi_lib.o -lm

pulsar.o:	pulsar.c
	$(CC) $(CFLAGS) -c pulsar.c


####

random:	random.o ../lib/spi_lib.o
	$(CC) $(LFLAGS) -o random random.o ../lib/spi_lib.o 

random.o:	random.c
	$(CC) $(CFLAGS) -c random.c


####

rainbow:	rainbow.o ../lib/spi_lib.o
	$(CC) $(LFLAGS) -o rainbow rainbow.o ../lib/spi_lib.o

rainbow.o:	rainbow.c
	$(CC) $(CFLAGS) -c rainbow.c


####

red_green:	red_green.o ../lib/spi_lib.o
	$(CC) $(LFLAGS) -o red_green red_green.o ../lib/spi_lib.o

red_green.o:	red_green.c
	$(CC) $(CFLAGS) -c red_green.c


####

clean:	
	rm -f *.o *~ blue_yellow disable dual_scanner \
		led_test led_scanner pulsar rainbow random red_green

