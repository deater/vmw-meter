CC = gcc
CFLAGS = -O2 -Wall
LFLAGS = -lm

TEXT_TO_YM = ../conversion_tools/text_to_ym
YM_TO_YM5 = ../conversion_tools/ym_to_ym5
TEXT_TO_ED = ../conversion_tools/text_to_ed

all:	sa.ym5 sa.ed \
	kerbal.ym5 \
	korobeiniki.ym5 \
	fighting.ym5 fighting.ed \
	highwind.ym5 highwind.ed \
	test.ed

#	saxm1.ym5 saxm2.ym5 saxm3.ym5 saxm.vmw5

###

saxm.vmw5:	saxm1.ym5 saxm3.ym5
	cp saxm1.ym5 saxm.vmw5
	cat saxm3.ym5 >> saxm.vmw5

###


saxm1.txt:	../xm_to_text ../stillalive.xm
	../xm_to_text -c 0,2,3 ../stillalive.xm ./saxm1.txt

saxm1.ym:	$(TEXT_TO_YM) saxm1.txt
	$(TEXT_TO_YM) saxm1.txt saxm1

saxm1.ym5:	saxm1.ym
	$(YM_TO_YM5) saxm1

###

saxm2.txt:	../xm_to_text ../stillalive.xm
	../xm_to_text -c 0,1,2 ../stillalive.xm ./saxm2.txt

saxm2.ym:	$(TEXT_TO_YM) saxm2.txt
	$(TEXT_TO_YM) saxm2.txt saxm2

saxm2.ym5:	saxm2.ym
	$(YM_TO_YM5) saxm2

###

saxm3.txt:	../xm_to_text ../stillalive.xm
	../xm_to_text -c 0,1,3 ../stillalive.xm ./saxm3.txt

saxm3.ym:	$(TEXT_TO_YM) saxm3.txt
	$(TEXT_TO_YM) saxm3.txt saxm3

saxm3.ym5:	saxm3.ym
	$(YM_TO_YM5) saxm3

###


fighting.ed:	$(TEXT_TO_ED) fighting.txt
	$(TEXT_TO_ED) fighting.txt fighting

fighting.ym:	$(TEXT_TO_YM) fighting.txt
	$(TEXT_TO_YM) fighting.txt fighting

fighting.ym5:	$(YM_TO_YM5) fighting.ym
	$(YM_TO_YM5) fighting

###

highwind.ed:	$(TEXT_TO_ED) highwind.txt
	$(TEXT_TO_ED) highwind.txt highwind

highwind.ym:	$(TEXT_TO_YM) highwind.txt
	$(TEXT_TO_YM) highwind.txt highwind

highwind.ym5:	$(YM_TO_YM5) highwind.ym
	$(YM_TO_YM5) highwind


###

test.ed:	$(TEXT_TO_ED) test.txt
	$(TEXT_TO_ED) test.txt test

###

kerbal.ym:	$(TEXT_TO_YM) kerbal.txt
	$(TEXT_TO_YM) kerbal.txt kerbal

kerbal.ym5:	$(YM_TO_YM5) kerbal.ym
	$(YM_TO_YM5) kerbal

###

korobeiniki.ym:	$(TEXT_TO_YM) korobeiniki.txt
	$(TEXT_TO_YM) korobeiniki.txt korobeiniki

korobeiniki.ym5:	$(YM_TO_YM5) korobeiniki.ym
	$(YM_TO_YM5) korobeiniki

###

sa.ed:	$(TEXT_TO_ED) sa.txt
	$(TEXT_TO_ED) sa.txt sa

sa.ym:	$(TEXT_TO_YM) sa.txt
	$(TEXT_TO_YM) sa.txt sa

sa.ym5:	$(YM_TO_YM5) sa.ym
	$(YM_TO_YM5) sa



clean:	
	rm -f *~  rm *.ym5 *.ym *.lyrics *.ed *.edlyrics saxm.txt
