CC = gcc
CFLAGS = -O2 -Wall
LFLAGS = -lm

all:	sa.ym5 korobeiniki.ym5 fighting.ym5 highwind.ym5 \
	highwind.ed fighting.ed sa.ed sa_chiptune.ym5

###


fighting.ed:	../text_to_ed fighting.txt
	../text_to_ed fighting.txt fighting

fighting.ym:	../text_to_ym fighting.txt
	../text_to_ym fighting.txt fighting

fighting.ym5:	../ym_to_ym5 fighting.ym
	../ym_to_ym5 fighting

###

highwind.ed:	../text_to_ed highwind.txt
	../text_to_ed highwind.txt highwind

highwind.ym:	../text_to_ym highwind.txt
	../text_to_ym highwind.txt highwind

highwind.ym5:	../ym_to_ym5 highwind.ym
	../ym_to_ym5 highwind


###

korobeiniki.ym:	../text_to_ym korobeiniki.txt
	../text_to_ym korobeiniki.txt korobeiniki

korobeiniki.ym5:	../ym_to_ym5 korobeiniki.ym
	../ym_to_ym5 korobeiniki

###

sa.ed:	../text_to_ed sa.txt
	../text_to_ed sa.txt sa

sa.ym:	../text_to_ym sa.txt
	../text_to_ym sa.txt sa

sa.ym5:	../ym_to_ym5 sa.ym
	../ym_to_ym5 sa


sa_chiptune.ym:	../text_to_ym sa_chiptune.txt
	../text_to_ym sa_chiptune.txt sa_chiptune

sa_chiptune.ym5:	../ym_to_ym5 sa_chiptune.ym
	../ym_to_ym5 sa_chiptune


clean:	
	rm -f *~  rm *.ym5 *.ym *.lyrics *.ed
