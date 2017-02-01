CC = gcc
CFLAGS = -O2 -Wall
LFLAGS = -lm

all:	sa.ym5

sa.ym:	../text_to_ym sa.txt
	../text_to_ym < sa.txt

sa.ym5:	../ym_to_ym5 sa.ym
	../ym_to_ym5


clean:	
	rm -f *~ 

