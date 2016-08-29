CC = gcc
CFLAGS = -O2 -Wall -I../include
LFLASG =

all:	compact_log dump_log read_adc calc_power log_power jgraph_log

../lib/spi_lib.o:	../lib/spi_lib.c
	cd ../lib && make

calc_power:	calc_power.o ../lib/spi_lib.o
	$(CC) $(LFLAGS) -o calc_power calc_power.o ../lib/spi_lib.o

calc_power.o:	calc_power.c
	$(CC) $(CFLAGS) -c calc_power.c


compact_log:	compact_log.o ../lib/spi_lib.o
	$(CC) $(LFLAGS) -o compact_log compact_log.o ../lib/spi_lib.o

compact_log.o:	compact_log.c
	$(CC) $(CFLAGS) -c compact_log.c


dump_log:	dump_log.o ../lib/spi_lib.o
	$(CC) $(LFLAGS) -o dump_log dump_log.o ../lib/spi_lib.o

dump_log.o:	dump_log.c
	$(CC) $(CFLAGS) -c dump_log.c


jgraph_log:	jgraph_log.o ../lib/spi_lib.o
	$(CC) $(LFLAGS) -o jgraph_log jgraph_log.o ../lib/spi_lib.o

jgraph_log.o:	jgraph_log.c
	$(CC) $(CFLAGS) -c jgraph_log.c



log_power:	log_power.o ../lib/spi_lib.o
	$(CC) $(LFLAGS) -o log_power log_power.o ../lib/spi_lib.o

log_power.o:	log_power.c
	$(CC) $(CFLAGS) -c log_power.c



read_adc:	read_adc.o ../lib/spi_lib.o
	$(CC) $(LFLAGS) -o read_adc read_adc.o ../lib/spi_lib.o

read_adc.o:	read_adc.c
	$(CC) $(CFLAGS) -c read_adc.c


clean:	
	rm -f *.o *~ read_adc calc_power compact_log log_power dump_log jgraph_log


