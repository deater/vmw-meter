CC = gcc
CFLAGS = -Wall -O2 -g -I../include
LFLAGS =

all:	test_display

####

test_display:	test_display.o
	$(CC) $(LFLAGS) -o test_display test_display.o

test_display.o:	test_display.c
	$(CC) $(CFLAGS) -c test_display.c

####

clean:	
	rm -f *~ *.o \
		test_display

