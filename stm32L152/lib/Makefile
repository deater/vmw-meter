-include ../Makefile.inc

all:	lcd_lib.o delay_lib.o num_to_string.o

delay_lib.o:	delay_lib.c
	$(CC) $(CFLAGS) -c delay_lib.c

lcd_lib.o:	lcd_lib.c
	$(CC) $(CFLAGS) -c lcd_lib.c

num_to_string.o:	num_to_string.c
	$(CC) $(CFLAGS) -c num_to_string.c

clean:	
	rm -f *~ *.o

