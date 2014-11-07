CC = gcc
CFLAGS = -Wall -O2
LFLAGS =

all:	temp_log

temp_log:	temp_log.o
	$(CC) $(LFLAGS) -o temp_log temp_log.o

temp_log.o:	temp_log.c
	$(CC) $(CFLAGS) -c temp_log.c


clean:	
	rm -f *.o *~ temp_log
