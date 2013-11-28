CC = gcc
CFLAGS = -Wall -O2
LFLAGS =

all:	test.h

test.h:	make_pov test
	./make_pov < test > test.h

make_pov:	make_pov.o
	$(CC) $(LFLAGS) -o make_pov make_pov.o

make_pov.o:	make_pov.c
	$(CC) $(CFLAGS) -c make_pov.c

clean:	
	rm -f make_pov test.h *.o *~

