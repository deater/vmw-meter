CC = gcc
CFLAGS = -Wall -O2 -I../include
LFLAGS =

all:	gumstix_gpio rpi_gpio_blink rpi_gpio_input rpi_gpio_input_int

../lib/gpio_lib.o:	../lib/gpio_lib.c ../include/gpio_lib.h
	cd ../lib && make

gumstix_gpio:	gumstix_gpio.o ../lib/gpio_lib.o
	$(CC) $(LFLAGS) -o gumstix_gpio gumstix_gpio.o ../lib/gpio_lib.o

gumstix_gpio.o:	gumstix_gpio.c
	$(CC) $(CFLAGS) -c gumstix_gpio.c


rpi_gpio_blink:	rpi_gpio_blink.o ../lib/gpio_lib.o
	$(CC) $(LFLAGS) -o rpi_gpio_blink rpi_gpio_blink.o ../lib/gpio_lib.o

rpi_gpio_blink.o:	rpi_gpio_blink.c
	$(CC) $(CFLAGS) -c rpi_gpio_blink.c



rpi_gpio_input:	rpi_gpio_input.o ../lib/gpio_lib.o
	$(CC) $(LFLAGS) -o rpi_gpio_input rpi_gpio_input.o ../lib/gpio_lib.o

rpi_gpio_input.o:	rpi_gpio_input.c
	$(CC) $(CFLAGS) -c rpi_gpio_input.c


rpi_gpio_input_int:	rpi_gpio_input_int.o ../lib/gpio_lib.o
	$(CC) $(LFLAGS) -o rpi_gpio_input_int rpi_gpio_input_int.o ../lib/gpio_lib.o

rpi_gpio_input_int.o:	rpi_gpio_input_int.c
	$(CC) $(CFLAGS) -c rpi_gpio_input_int.c


clean:	
	rm -f *~ *.o gumstix_gpio rpi_gpio_blink rpi_gpio_input rpi_gpio_input_int

