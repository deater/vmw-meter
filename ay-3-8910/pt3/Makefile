CC = gcc
CFLAGS = -g -O2 -Wall

all:	decode_pt3 pt3_to_ym5


out.ym5:	out.ym
	../conversion_tools/ym_to_ym5 out

out.ym:	pt3_to_ym5
	./pt3_to_ym5 ar_it.pt3

###

decode_pt3:	decode_pt3.o pt3_lib.o pt3_lib_text.o
	$(CC) $(LFLAGS) -o decode_pt3 decode_pt3.o pt3_lib.o pt3_lib_text.o

decode_pt3.o:	decode_pt3.c pt3_lib.h
	$(CC) $(CFLAGS) -c decode_pt3.c

###

pt3_to_ym5:	pt3_to_ym5.o pt3_lib.o pt3_lib_text.o
	$(CC) $(LFLAGS) -o pt3_to_ym5 pt3_to_ym5.o pt3_lib.o pt3_lib_text.o

pt3_to_ym5.o:	pt3_to_ym5.c pt3_lib.h
	$(CC) $(CFLAGS) -c pt3_to_ym5.c

###

pt3_lib.o:	pt3_lib.c pt3_lib.h
	$(CC) $(CFLAGS) -c pt3_lib.c

pt3_lib_text.o:	pt3_lib_text.c pt3_lib_text.h
	$(CC) $(CFLAGS) -c pt3_lib_text.c

###

clean:
	rm -f *.o *~ decode_pt3 pt3_to_ym5
