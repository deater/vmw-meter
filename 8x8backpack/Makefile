CC = gcc
CFLAGS = -Wall -O2 -g
LFLAGS =

all:	ball_bounce_demo clock_demo fill_demo flame_demo font_demo \
	sine_demo spiral_demo starfield_demo

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

clock_demo:	clock_demo.o i2c_lib.o graphics_lib.o
	$(CC) $(LFLASG) -o clock_demo clock_demo.o i2c_lib.o graphics_lib.o

clock_demo.o:	clock_demo.c i2c_lib.h graphics_lib.h
	$(CC) $(CFLAGS) -c clock_demo.c


####

fill_demo:	fill_demo.o i2c_lib.o
	$(CC) $(LFLASG) -o fill_demo fill_demo.o i2c_lib.o

fill_demo.o:	fill_demo.c i2c_lib.h
	$(CC) $(CFLAGS) -c fill_demo.c

####

flame_demo:	flame_demo.o i2c_lib.o graphics_lib.o
	$(CC) $(LFLASG) -o flame_demo flame_demo.o i2c_lib.o graphics_lib.o

flame_demo.o:	flame_demo.c i2c_lib.h
	$(CC) $(CFLAGS) -c flame_demo.c


####

font_demo:	font_demo.o i2c_lib.o graphics_lib.o
	$(CC) $(LFLASG) -o font_demo font_demo.o \
			i2c_lib.o graphics_lib.o

font_demo.o:	font_demo.c i2c_lib.h graphics_lib.h
	$(CC) $(CFLAGS) -c font_demo.c


####

sine_demo:	sine_demo.o i2c_lib.o graphics_lib.o
	$(CC) $(LFLASG) -o sine_demo sine_demo.o \
			i2c_lib.o graphics_lib.o -lm

sine_demo.o:	sine_demo.c i2c_lib.h graphics_lib.h
	$(CC) $(CFLAGS) -c sine_demo.c


####

spiral_demo:	spiral_demo.o i2c_lib.o graphics_lib.o
	$(CC) $(LFLASG) -o spiral_demo spiral_demo.o \
			i2c_lib.o graphics_lib.o

spiral_demo.o:	spiral_demo.c i2c_lib.h graphics_lib.h
	$(CC) $(CFLAGS) -c spiral_demo.c


####

starfield_demo:	starfield_demo.o i2c_lib.o graphics_lib.o
	$(CC) $(LFLASG) -o starfield_demo starfield_demo.o \
			i2c_lib.o graphics_lib.o

starfield_demo.o:	starfield_demo.c i2c_lib.h graphics_lib.h
	$(CC) $(CFLAGS) -c starfield_demo.c


####

clean:	
	rm -f *~ *.o \
		ball_bounce_demo clock_demo fill_demo flame_demo \
		font_demo \
		sine_demo spiral_demo starfield_demo

