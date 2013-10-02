CC = gcc
CFLAGS = -O2 -Wall -g

all:	i2c_lib.o

i2c_lib.o:	i2c_lib.c i2c_lib.h
	$(CC) $(CFLAGS) -c i2c_lib.c

clean:	
	rm -f *.o *~

