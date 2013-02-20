CC = gcc
CFLAGS = -O2 -Wall
LFLAGS = 

all:	ledmeter

ledmeter:	ledmeter.o
		$(CC) $(LDFLAGS) -o ledmeter ledmeter.o
		
ledmeter.o:	ledmeter.c
		$(CC) $(CFLAGS) -c ledmeter.c
		
		
clean:		
		rm -f *.o *~ ledmeter
		
