CC = gcc
CFLAGS = -O2 -Wall -I../lib
LFLASG =

all:	led_test led_scanner

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

clean:	
	rm -f *.o *~ led_test led_scanner

