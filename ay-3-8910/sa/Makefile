CC = gcc
CFLAGS = -O2 -Wall
LFLAGS = -lm

all:	text_to_ym

text_to_ym:	text_to_ym.o
	$(CC) $(LFLAGS) -o text_to_ym text_to_ym.o

text_to_ym.o:	text_to_ym.c
	$(CC) $(CFLAGS) -c text_to_ym.c

clean:	
	rm -f *~ *.o text_to_ym

