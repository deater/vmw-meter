CC = gcc
CFLAGS = -O2 -Wall -I../../lib
LFLAGS =

all:	char_test \
        clock_demo \
	cpu_meter \
	enemy_attack \
	music_demo \
	pulse_demo \
        scroll_string \
	spin_demo \
	string_demo \
	set_display 

###

14seg_font.o:	14seg_font.c 14seg_font.h
	$(CC) $(CFLAGS) -c 14seg_font.c

###

../../lib/i2c_lib.o:	../../lib/i2c_lib.c ../../lib/i2c_lib.h
	cd ../../lib && make

###

char_test:	char_test.o 14seg_font.o
	$(CC) $(LFLAGS) -o char_test char_test.o 14seg_font.o

char_test.o:	char_test.c ../../lib/i2c_lib.h
	$(CC) $(CFLAGS) -c char_test.c

###

clock_demo:	clock_demo.o 14seg_font.o ../../lib/i2c_lib.o
	$(CC) $(LFLAGS) -o clock_demo clock_demo.o 14seg_font.o ../../lib/i2c_lib.o

clock_demo.o:	clock_demo.c 14seg_font.h ../../lib/i2c_lib.h
	$(CC) $(CFLAGS) -c clock_demo.c


###

cpu_meter:	cpu_meter.o 14seg_font.o ../../lib/i2c_lib.o
	$(CC) $(LFLAGS) -o cpu_meter cpu_meter.o 14seg_font.o ../../lib/i2c_lib.o

cpu_meter.o:	cpu_meter.c 14seg_font.h ../../lib/i2c_lib.h
	$(CC) $(CFLAGS) -c cpu_meter.c	

###

enemy_attack:	enemy_attack.o 14seg_font.o ../../lib/i2c_lib.o
	$(CC) $(LFLAGS) -o enemy_attack enemy_attack.o 14seg_font.o ../../lib/i2c_lib.o

enemy_attack.o:	enemy_attack.c 14seg_font.h ../../lib/i2c_lib.h
	$(CC) $(CFLAGS) -c enemy_attack.c	


###

music_demo:	music_demo.o 14seg_font.o ../../lib/i2c_lib.o
	$(CC) $(LFLAGS) -o music_demo music_demo.o 14seg_font.o ../../lib/i2c_lib.o -lm

music_demo.o:	music_demo.c ../../lib/i2c_lib.h
	$(CC) $(CFLAGS) -c music_demo.c	

###

pulse_demo:	pulse_demo.o ../../lib/i2c_lib.o
	$(CC) $(LFLAGS) -o pulse_demo pulse_demo.o ../../lib/i2c_lib.o

pulse_demo.o:	pulse_demo.c ../../lib/i2c_lib.h
	$(CC) $(CFLAGS) -c pulse_demo.c

###

scroll_string:	scroll_string.o 14seg_font.o ../../lib/i2c_lib.o
	$(CC) $(LFLAGS) -o scroll_string scroll_string.o 14seg_font.o ../../lib/i2c_lib.o

scroll_string.o:	scroll_string.c ../../lib/i2c_lib.h
	$(CC) $(CFLAGS) -c scroll_string.c


###

spin_demo:	spin_demo.o ../../lib/i2c_lib.o
	$(CC) $(LFLAGS) -o spin_demo spin_demo.o ../../lib/i2c_lib.o	

spin_demo.o:	spin_demo.c ../../lib/i2c_lib.h
	$(CC) $(CFLAGS) -c spin_demo.c


###

string_demo:	string_demo.o 14seg_font.o ../../lib/i2c_lib.o
	$(CC) $(LFLAGS) -o string_demo string_demo.o 14seg_font.o ../../lib/i2c_lib.o

string_demo.o:	string_demo.c ../../lib/i2c_lib.h
	$(CC) $(CFLAGS) -c string_demo.c



###

set_display:	set_display.o 14seg_font.o ../../lib/i2c_lib.o
	$(CC) $(LFLAGS) -o set_display set_display.o 14seg_font.o ../../lib/i2c_lib.o

set_display.o:	set_display.c ../../lib/i2c_lib.h
	$(CC) $(CFLAGS) -c set_display.c	







clean:	
	rm -f *.o *~ char_test \
	clock_demo spin_demo pulse_demo string_demo \
	scroll_string cpu_meter music_demo \
	set_display enemy_attack
