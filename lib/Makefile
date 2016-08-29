CC = gcc
CFLAGS = -O2 -Wall -g -I../include

all:	i2c_lib.o gpio_lib.o graphics_lib.o spi_lib.o

i2c_lib.o:	i2c_lib.c ../include/i2c_lib.h
	$(CC) $(CFLAGS) -c i2c_lib.c

gpio_lib.o:	gpio_lib.c ../include/gpio_lib.h
	$(CC) $(CFLAGS) -c gpio_lib.c

graphics_lib.o:	graphics_lib.c ../include/graphics_lib.h
	$(CC) $(CFLAGS) -c graphics_lib.c

spi_lib.o:	spi_lib.c ../include/spi_lib.h
	$(CC) $(CFLAGS) -c spi_lib.c

clean:	
	rm -f *.o *~

