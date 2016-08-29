CC = gcc
CFLAGS = -Wall -O2 -I../include
LFLAGS =

all:	shift_led_test

../lib/gpio_lib.o:	../lib/gpio_lib.c ../include/gpio_lib.h
	cd ../lib && make

shift_led_test:	shift_led_test.o ../lib/gpio_lib.o
	$(CC) $(LFLAGS) -o shift_led_test shift_led_test.o ../lib/gpio_lib.o

shift_led_test.o:	shift_led_test.c
	$(CC) $(CFLAGS) -c shift_led_test.c


clean:	
	rm -f *~ *.o shift_led_test

