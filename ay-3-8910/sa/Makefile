CC = gcc
CFLAGS = -O2 -Wall
LFLAGS = -lm

TEXT_TO_YM = ../conversion_tools/text_to_ym
YM_TO_YM5 = ../conversion_tools/ym_to_ym5

all:	saxm.vmw5

###

saxm.vmw5:	saxm1.ym5 saxm3.ym5
	cp saxm1.ym5 saxm.vmw5
	cat saxm3.ym5 >> saxm.vmw5

###

saxm1.ym:	saxm1.txt
	$(TEXT_TO_YM) saxm1.txt saxm1

saxm1.ym5:	saxm1.ym
	$(YM_TO_YM5) saxm1

###

saxm3.ym:	saxm3.txt
	$(TEXT_TO_YM) saxm3.txt saxm3

saxm3.ym5:	saxm3.ym
	$(YM_TO_YM5) saxm3

clean:	
	rm -f *~
#rm *.ym5 *.ym *.ed saxm.vmw5
