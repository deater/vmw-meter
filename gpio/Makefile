CC = gcc
CFLAGS = -Wall -O2
LFLAGS =

all:	gumstix_gpio

gumstix_gpio:	gumstix_gpio.o
	$(CC) $(LFLAGS) -o gumstix_gpio gumstix_gpio.o

gumstix_gpio.o:	gumstix_gpio.c
	$(CC) $(CFLAGS) -c gumstix_gpio.c

clean:	
	rm -f *~ *.o gumstix_gpio


