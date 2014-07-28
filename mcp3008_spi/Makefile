CC = gcc
CFLAGS = -O2 -Wall
LFLASG =

all:	read_adc

read_adc:	read_adc.o spi_lib.o
	$(CC) $(LFLAGS) -o read_adc read_adc.o spi_lib.o

read_adc.o:	read_adc.c
	$(CC) $(CFLAGS) -c read_adc.c


spi_lib.o:	spi_lib.c
	$(CC) $(CFLAGS) -c spi_lib.c

clean:	
	rm -f *.o *~ read_adc

