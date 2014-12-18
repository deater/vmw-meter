CC = gcc
CFLAGS = -O2 -Wall -g

all:	i2c_lib.o graphics_lib.o spi_lib.o

i2c_lib.o:	i2c_lib.c i2c_lib.h
	$(CC) $(CFLAGS) -c i2c_lib.c

graphics_lib.o:	graphics_lib.c graphics_lib.h
	$(CC) $(CFLAGS) -c graphics_lib.c

spi_lib.o:	spi_lib.c spi_lib.h
	$(CC) $(CFLAGS) -c spi_lib.c

clean:	
	rm -f *.o *~

