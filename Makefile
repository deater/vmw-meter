CC = gcc
CFLAGS = -O2 -Wall
LFLAGS =

all:	char_test \
        clock_demo.ascii clock_demo.i2c \
	cpu_meter.ascii cpu_meter.i2c \
	music_demo.ascii music_demo.i2c \
	pulse_demo.ascii pulse_demo.i2c \
	spin_demo.ascii spin_demo.i2c \
	string_demo.ascii string_demo.i2c \
        scroll_string.ascii scroll_string.i2c
	

ascii:	clock_demo.ascii pulse_demo.ascii spin_demo.ascii string_demo.ascii

i2c:	clock_demo.i2c pulse_demo.i2c spin_demo.i2c string_demo.i2c

char_test:	char_test.o meter_tools.o meter-ascii.o
	$(CC) $(LFLAGS) -o char_test char_test.o meter_tools.o meter-ascii.o

char_test.o:	char_test.c meter.h meter_tools.h
	$(CC) $(CFLAGS) -c char_test.c


meter-ascii.o:	meter-ascii.c meter.h
	$(CC) $(CFLAGS) -c meter-ascii.c

meter-i2c.o:	meter-i2c.c meter.h
	$(CC) $(CFLAGS) -c meter-i2c.c	


meter_tools.o:	meter_tools.c meter_tools.h
	$(CC) $(CFLAGS) -c meter_tools.c


clock_demo.ascii:	clock_demo.o meter-ascii.o meter_tools.o
	$(CC) $(LFLAGS) -o clock_demo.ascii clock_demo.o meter-ascii.o meter_tools.o

clock_demo.i2c:	clock_demo.o meter-i2c.o meter_tools.o
	$(CC) $(LFLAGS) -o clock_demo.i2c clock_demo.o meter-i2c.o meter_tools.o

clock_demo.o:	clock_demo.c meter.h meter_tools.h
	$(CC) $(CFLAGS) -c clock_demo.c
	
	
cpu_meter.ascii:	cpu_meter.o meter-ascii.o meter_tools.o
	$(CC) $(LFLAGS) -o cpu_meter.ascii cpu_meter.o meter-ascii.o meter_tools.o

cpu_meter.i2c:	cpu_meter.o meter-i2c.o meter_tools.o
	$(CC) $(LFLAGS) -o cpu_meter.i2c cpu_meter.o meter-i2c.o meter_tools.o

cpu_meter.o:	cpu_meter.c meter.h meter_tools.h
	$(CC) $(CFLAGS) -c cpu_meter.c	
	
	
music_demo.ascii:	music_demo.o meter-ascii.o meter_tools.o
	$(CC) $(LFLAGS) -o music_demo.ascii music_demo.o meter-ascii.o meter_tools.o -lm

music_demo.i2c:	music_demo.o meter-i2c.o meter_tools.o
	$(CC) $(LFLAGS) -o music_demo.i2c music_demo.o meter-i2c.o meter_tools.o -lm

music_demo.o:	music_demo.c meter.h meter_tools.h
	$(CC) $(CFLAGS) -c music_demo.c	
	
	

pulse_demo.ascii:	pulse_demo.o meter-ascii.o meter_tools.o
	$(CC) $(LFLAGS) -o pulse_demo.ascii pulse_demo.o meter-ascii.o meter_tools.o

pulse_demo.i2c:	pulse_demo.o meter-i2c.o meter_tools.o
	$(CC) $(LFLAGS) -o pulse_demo.i2c pulse_demo.o meter-i2c.o meter_tools.o	

pulse_demo.o:	pulse_demo.c meter.h meter_tools.h
	$(CC) $(CFLAGS) -c pulse_demo.c


spin_demo.ascii:	spin_demo.o meter-ascii.o meter_tools.o
	$(CC) $(LFLAGS) -o spin_demo.ascii spin_demo.o meter-ascii.o meter_tools.o

spin_demo.i2c:	spin_demo.o meter-i2c.o meter_tools.o
	$(CC) $(LFLAGS) -o spin_demo.i2c spin_demo.o meter-i2c.o meter_tools.o	

spin_demo.o:	spin_demo.c meter.h meter_tools.h
	$(CC) $(CFLAGS) -c spin_demo.c


string_demo.ascii:	string_demo.o meter-ascii.o meter_tools.o
	$(CC) $(LFLAGS) -o string_demo.ascii string_demo.o meter-ascii.o meter_tools.o

string_demo.i2c:	string_demo.o meter-i2c.o meter_tools.o
	$(CC) $(LFLAGS) -o string_demo.i2c string_demo.o meter-i2c.o meter_tools.o

string_demo.o:	string_demo.c meter.h meter_tools.h
	$(CC) $(CFLAGS) -c string_demo.c


scroll_string.ascii:	scroll_string.o meter-ascii.o meter_tools.o
	$(CC) $(LFLAGS) -o scroll_string.ascii scroll_string.o meter-ascii.o meter_tools.o

scroll_string.i2c:	scroll_string.o meter-i2c.o meter_tools.o
	$(CC) $(LFLAGS) -o scroll_string.i2c scroll_string.o meter-i2c.o meter_tools.o

scroll_string.o:	scroll_string.c meter.h meter_tools.h
	$(CC) $(CFLAGS) -c scroll_string.c


clean:	
	rm -f *.o *~ char_test \
	clock_demo.ascii spin_demo.ascii pulse_demo.ascii string_demo.ascii \
	clock_demo.i2c spin_demo.i2c pulse_demo.i2c string_demo.i2c \
	scroll_string.ascii scroll_string.i2c \
	cpu_meter.i2c cpu_meter.ascii \
	music_demo.i2c music_demo.ascii
