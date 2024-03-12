CC = gcc
CFLAGS = -Wall -O2 -g -I../include
LFLAGS =

all:	test_display disable_display scanner

####

scanner:	scanner.o
	$(CC) $(LFLAGS) -o scanner scanner.o

scanner.o:	scanner.c is31fl3731.h
	$(CC) $(CFLAGS) -c scanner.c

####

test_display:	test_display.o
	$(CC) $(LFLAGS) -o test_display test_display.o

test_display.o:	test_display.c is31fl3731.h
	$(CC) $(CFLAGS) -c test_display.c

####

disable_display:	disable_display.o
	$(CC) $(LFLAGS) -o disable_display disable_display.o

disable_display.o:	disable_display.c is31fl3731.h
	$(CC) $(CFLAGS) -c disable_display.c

####

clean:	
	rm -f *~ *.o \
		test_display disable_display

