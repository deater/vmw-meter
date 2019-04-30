CC = gcc
CFLAGS = -g -O2 -Wall

all:	decode_pt3 pt3_to_ym5

###

decode_pt3:	decode_pt3.o
	$(CC) $(LFLAGS) -o decode_pt3 decode_pt3.o

decode_pt3.o:	decode_pt3.c
	$(CC) $(CFLAGS) -c decode_pt3.c

###

pt3_to_ym5:	pt3_to_ym5.o
	$(CC) $(LFLAGS) -o pt3_to_ym5 pt3_to_ym5.o

pt3_to_ym5.o:	pt3_to_ym5.c
	$(CC) $(CFLAGS) -c pt3_to_ym5.c

###

clean:
	rm -f *.o *~ decode_pt3 pt3_to_ym5
