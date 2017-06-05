CC = gcc
CFLAGS = -O2 -Wall
LFLAGS = -lm

all:	saxm.vmw5

###

saxm.vmw5:	saxm1.ym5 saxm3.ym5
	cp saxm1.ym5 saxm.vmw5
	cat saxm3.ym5 >> saxm.vmw5

###

saxm1.ym:	../text_to_ym saxm1.txt
	../text_to_ym saxm1.txt saxm1

saxm1.ym5:	saxm1.ym
	../ym_to_ym5 saxm1

###

saxm3.ym:	../text_to_ym saxm3.txt
	../text_to_ym saxm3.txt saxm3

saxm3.ym5:	saxm3.ym
	../ym_to_ym5 saxm3

clean:	
	rm -f *~  rm *.ym5 *.ym *.lyrics *.ed saxm.txt
