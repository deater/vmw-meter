CC = gcc
CFLAGS = -Wall -O2
LFLAGS =

all:	temp_log read_temp


###

temp_log:	temp_log.o
	$(CC) $(LFLAGS) -o temp_log temp_log.o

temp_log.o:	temp_log.c
	$(CC) $(CFLAGS) -c temp_log.c

###

read_temp:	read_temp.o
	$(CC) $(LFLAGS) -o read_temp read_temp.o

read_temp.o:	read_temp.c
	$(CC) $(CFLAGS) -c read_temp.c

###

clean:	
	rm -f *.o *~ temp_log read_temp
