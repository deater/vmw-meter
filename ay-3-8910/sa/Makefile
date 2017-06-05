CC = gcc
CFLAGS = -O2 -Wall
LFLAGS = -lm

all:	sa.ym5 korobeiniki.ym5 fighting.ym5 highwind.ym5 \
	highwind.ed fighting.ed sa.ed sa_chiptune.ym5 \
	saxm1.ym5 saxm2.ym5 saxm3.ym5 \
	saxm.vmw5

###

saxm.vmw5:	saxm1.ym5 saxm3.ym5
	cp saxm1.ym5 saxm.vmw5
	cat saxm3.ym5 >> saxm.vmw5

###


saxm1.txt:	../xm_to_text ../stillalive.xm
	../xm_to_text -c 0,2,3 ../stillalive.xm ./saxm1.txt

saxm1.ym:	../text_to_ym saxm1.txt
	../text_to_ym saxm1.txt saxm1

saxm1.ym5:	saxm1.ym
	../ym_to_ym5 saxm1

###

saxm2.txt:	../xm_to_text ../stillalive.xm
	../xm_to_text -c 0,1,2 ../stillalive.xm ./saxm2.txt

saxm2.ym:	../text_to_ym saxm2.txt
	../text_to_ym saxm2.txt saxm2

saxm2.ym5:	saxm2.ym
	../ym_to_ym5 saxm2

###

saxm3.txt:	../xm_to_text ../stillalive.xm
	../xm_to_text -c 0,1,3 ../stillalive.xm ./saxm3.txt

saxm3.ym:	../text_to_ym saxm3.txt
	../text_to_ym saxm3.txt saxm3

saxm3.ym5:	saxm3.ym
	../ym_to_ym5 saxm3

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
	rm -f *~  rm *.ym5 *.ym *.lyrics *.ed saxm.txt
