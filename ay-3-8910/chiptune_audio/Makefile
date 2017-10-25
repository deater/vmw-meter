include ../Makefile.include

all:	audio.a

audio.a:	ay-3-8910.o max98306.o
	$(AR) crus audio.a ay-3-8910.o max98306.o

ay-3-8910.o:	ay-3-8910.c ay-3-8910.h
	$(CC) $(CFLAGS) -c ay-3-8910.c

max98306.o:	max98306.c max98306.h
	$(CC) $(CFLAGS) -c max98306.c

clean:	
	rm -f *~ *.o *.a
