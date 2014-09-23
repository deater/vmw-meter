CC = gcc
CFLAGS = -Wall -O2
LFLAGS =

all:	gumstix_gpio rpi_gpio

gumstix_gpio:	gumstix_gpio.o
	$(CC) $(LFLAGS) -o gumstix_gpio gumstix_gpio.o

gumstix_gpio.o:	gumstix_gpio.c
	$(CC) $(CFLAGS) -c gumstix_gpio.c


rpi_gpio:	rpi_gpio.o
	$(CC) $(LFLAGS) -o rpi_gpio rpi_gpio.o

rpi_gpio.o:	rpi_gpio.c
	$(CC) $(CFLAGS) -c rpi_gpio.c


clean:	
	rm -f *~ *.o gumstix_gpio rpi_gpio


