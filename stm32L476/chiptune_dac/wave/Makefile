CC = gcc
CFLAGS = -Wall -O2 -g
LFLAGS = 

all:	wave convert make_fighting out.wav fighting.h highwind.h koro.h

out.wav:	make_fighting
	./make_fighting

make_fighting:	make_fighting.o
	$(CC) $(LFLAGS) -o make_fighting make_fighting.o

make_fighting.o:	make_fighting.c fighting.h
	$(CC) $(CFLAGS) -c make_fighting.c

fighting.h:	fighting.txt ./convert
	./convert < fighting.txt fighting > fighting.h

highwind.h:	highwind.txt ./convert
	./convert < highwind.txt highwind > highwind.h

koro.h:	korobeiniki.txt ./convert
	./convert < korobeiniki.txt koro > koro.h



wave:	wave.o
	$(CC) $(LFLAGS) -o wave wave.o

wave.o:	wave.c
	$(CC) $(CFLAGS) -c wave.c


convert:	convert.o
	$(CC) $(LFLAGS) -o convert convert.o

convert.o:	convert.c
	$(CC) $(CFLAGS) -c convert.c

clean:	
	rm -f *~ *.o wave convert make_fighting

