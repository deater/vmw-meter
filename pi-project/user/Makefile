include ../Makefile.inc

all:	shell.o vlibc.o

shell.o:	shell.c
	$(CROSS)$(CC) $(CFLAGS) -o shell.o -c shell.c

vlibc.o:	vlibc.c
	$(CROSS)$(CC) $(CFLAGS) -o vlibc.o -c vlibc.c

clean:
	rm -f *~ *.o *.map *.elf kernel.img *.dis


