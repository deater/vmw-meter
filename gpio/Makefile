CC = gcc
CFLAGS = -Wall -O2
LFLAGS =

all:	gumstix_gpio rpi_gpio_blink rpi_gpio_input

gpio_lib.o:	gpio_lib.c gpio_lib.h
	$(CC) $(CFLAGS) -c gpio_lib.c


gumstix_gpio:	gumstix_gpio.o gpio_lib.o
	$(CC) $(LFLAGS) -o gumstix_gpio gumstix_gpio.o gpio_lib.o

gumstix_gpio.o:	gumstix_gpio.c
	$(CC) $(CFLAGS) -c gumstix_gpio.c


rpi_gpio_blink:	rpi_gpio_blink.o gpio_lib.o
	$(CC) $(LFLAGS) -o rpi_gpio_blink rpi_gpio_blink.o gpio_lib.o

rpi_gpio_blink.o:	rpi_gpio_blink.c
	$(CC) $(CFLAGS) -c rpi_gpio_blink.c



rpi_gpio_input:	rpi_gpio_input.o gpio_lib.o
	$(CC) $(LFLAGS) -o rpi_gpio_input rpi_gpio_input.o gpio_lib.o

rpi_gpio_input.o:	rpi_gpio_input.c
	$(CC) $(CFLAGS) -c rpi_gpio_input.c


clean:	
	rm -f *~ *.o gumstix_gpio rpi_gpio_blink rpi_gpio_input


