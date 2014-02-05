-include ../Makefile.inc

all:	lcd_lib.o delay_lib.o

delay_lib.o:	delay_lib.c
	$(CC) $(CFLAGS) -c delay_lib.c

lcd_lib.o:	lcd_lib.c
	$(CC) $(CFLAGS) -c lcd_lib.c

clean:	
	rm -f *~ *.o

