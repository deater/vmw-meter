CC = gcc
CFLAGS = -O2 -Wall -I../lib
LFLASG =

all:	led_test led_scanner red_green rainbow

../lib/spi_lib.o:	../lib/spi_lib.c
	cd ../lib && make

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
	rm -f *.o *~ led_test led_scanner rainbow red_green

