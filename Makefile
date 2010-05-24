CC = gcc
CFLAGS = -O2 -Wall
LFLAGS =

all:	char_test clock_demo pulse_demo spin_demo string_demo

char_test:	char_test.o meter_tools.o meter.o
	$(CC) $(LFLAGS) -o char_test char_test.o meter_tools.o meter.o

char_test.o:	char_test.c meter.h meter_tools.h
	$(CC) $(CFLAGS) -c char_test.c

clock_demo:	clock_demo.o meter.o meter_tools.o
	$(CC) $(LFLAGS) -o clock_demo clock_demo.o meter.o meter_tools.o

clock_demo.o:	clock_demo.c meter.h meter_tools.h
	$(CC) $(CFLAGS) -c clock_demo.c

meter.o:	meter.c meter.h
	$(CC) $(CFLAGS) -c meter.c

meter_tools.o:	meter_tools.c meter_tools.h
	$(CC) $(CFLAGS) -c meter_tools.c

pulse_demo:	pulse_demo.o meter.o meter_tools.o
	$(CC) $(LFLAGS) -o pulse_demo pulse_demo.o meter.o meter_tools.o

pulse_demo.o:	pulse_demo.c meter.h meter_tools.h
	$(CC) $(CFLAGS) -c pulse_demo.c

spin_demo:	spin_demo.o meter.o meter_tools.o
	$(CC) $(LFLAGS) -o spin_demo spin_demo.o meter.o meter_tools.o

spin_demo.o:	spin_demo.c meter.h meter_tools.h
	$(CC) $(CFLAGS) -c spin_demo.c

string_demo:	string_demo.o meter.o meter_tools.o
	$(CC) $(LFLAGS) -o string_demo string_demo.o meter.o meter_tools.o

string_demo.o:	string_demo.c meter.h meter_tools.h
	$(CC) $(CFLAGS) -c string_demo.c

clean:	
	rm -f *.o *~ char_test clock_demo spin_demo pulse_demo string_demo
