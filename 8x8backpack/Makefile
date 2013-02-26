CC = gcc
CFLAGS = -Wall -O2
LFLAGS =

all:	ball_bounce_demo fill_demo spiral_demo

i2c_lib.o:	i2c_lib.c i2c_lib.h
	$(CC) $(CFLAGS) -c i2c_lib.c

graphics_lib.o:	graphics_lib.c graphics_lib.h
	$(CC) $(CFLAGS) -c graphics_lib.c

####

ball_bounce_demo:	ball_bounce_demo.o i2c_lib.o graphics_lib.o
	$(CC) $(LFLASG) -o ball_bounce_demo ball_bounce_demo.o \
			i2c_lib.o graphics_lib.o

ball_bounce_demo.o:	ball_bounce_demo.c i2c_lib.h graphics_lib.h
	$(CC) $(CFLAGS) -c ball_bounce_demo.c


####

fill_demo:	fill_demo.o i2c_lib.o
	$(CC) $(LFLASG) -o fill_demo fill_demo.o i2c_lib.o

fill_demo.o:	fill_demo.c i2c_lib.h
	$(CC) $(CFLAGS) -c fill_demo.c

####

spiral_demo:	spiral_demo.o i2c_lib.o graphics_lib.o
	$(CC) $(LFLASG) -o spiral_demo spiral_demo.o \
			i2c_lib.o graphics_lib.o

spiral_demo.o:	spiral_demo.c i2c_lib.h graphics_lib.h
	$(CC) $(CFLAGS) -c spiral_demo.c

####

clean:	
	rm -f *~ *.o ball_bounce_demo fill_demo spiral_demo

