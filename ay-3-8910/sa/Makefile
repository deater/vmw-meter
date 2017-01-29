CC = gcc
CFLAGS = -O2 -Wall
LFLAGS = -lm

all:	ym_to_ym5 text_to_ym sa.ym sa.ym5

sa.ym:	text_to_ym sa.txt
	./text_to_ym < sa.txt

sa.ym5:	ym_to_ym5 sa.ym
	./ym_to_ym5


text_to_ym:	text_to_ym.o
	$(CC) $(LFLAGS) -o text_to_ym text_to_ym.o

text_to_ym.o:	text_to_ym.c
	$(CC) $(CFLAGS) -c text_to_ym.c


ym_to_ym5:	ym_to_ym5.o
	$(CC) $(LFLAGS) -o ym_to_ym5 ym_to_ym5.o

ym_to_ym5.o:	ym_to_ym5.c
	$(CC) $(CFLAGS) -c ym_to_ym5.c



clean:	
	rm -f *~ *.o text_to_ym ym_to_ym5

