CC = gcc
CFLAGS = -O2 -Wall
LFLAGS = -lm

all:	sa.ym5 korobeiniki.ym5


korobeiniki.ym:	../text_to_ym korobeiniki.txt
	../text_to_ym korobeiniki.txt korobeiniki

korobeiniki.ym5:	../ym_to_ym5 korobeiniki.ym
	../ym_to_ym5 korobeiniki

sa.ym:	../text_to_ym sa.txt
	../text_to_ym sa.txt sa

sa.ym5:	../ym_to_ym5 sa.ym
	../ym_to_ym5 sa


clean:	
	rm -f *~  rm sa.ym5 sa.ym korobeiniki.ym5 korobeiniki.ym
