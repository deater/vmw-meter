CC = gcc
CFLAGS = -O2 -Wall -I../lib
LFLASG =

all:	read_adc calc_power log_power

../lib/spi_lib.o:	../lib/spi_lib.c
	cd ../lib && make

calc_power:	calc_power.o ../lib/spi_lib.o
	$(CC) $(LFLAGS) -o calc_power calc_power.o ../lib/spi_lib.o

calc_power.o:	calc_power.c
	$(CC) $(CFLAGS) -c calc_power.c


log_power:	log_power.o ../lib/spi_lib.o
	$(CC) $(LFLAGS) -o log_power log_power.o ../lib/spi_lib.o

log_power.o:	log_power.c
	$(CC) $(CFLAGS) -c log_power.c



read_adc:	read_adc.o ../lib/spi_lib.o
	$(CC) $(LFLAGS) -o read_adc read_adc.o ../lib/spi_lib.o

read_adc.o:	read_adc.c
	$(CC) $(CFLAGS) -c read_adc.c


clean:	
	rm -f *.o *~ read_adc calc_power log_power

