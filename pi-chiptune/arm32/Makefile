AS = as
CC = gcc
LD = ld

#AS = arm-none-eabi-as
#CC = arm-none-eabi-gcc
#LD = arm-none-eabi-ld
#OBJCOPY = arm-none-eabi-objcopy

CFLAGS = -g -O2 -Wall
LFLAGS = -lasound

all:	test_pt3

-include ../Makefile.lib

chiptune.elf:	chiptune.o lcd.o pt3_lib.o string.o ay8912.o ldivmod.o uldivmod.o
	$(LD) $(LINKER_SCRIPT) -nostartfiles -o chiptune.elf chiptune.o lcd.o pt3_lib.o string.o ay8912.o ldivmod.o uldivmod.o

####

test_pt3:	test_pt3.o ay8912.o pt3_lib.o
	$(CC) -o test_pt3 test_pt3.o ay8912.o pt3_lib.o $(LFLAGS)

test_pt3.o:	test_pt3.c
	$(CC) $(CFLAGS) -c test_pt3.c

ay8912.o:	ay8912.c
	$(CC) $(CFLAGS) -o ay8912.o -c ay8912.c

pt3_lib.o:	pt3_lib.c
	$(CC) $(CFLAGS) -o pt3_lib.o -c pt3_lib.c

####

ldivmod.o:	ldivmod.s
	$(AS) -c -o ldivmod.o ldivmod.s

uldivmod.o:	uldivmod.s
	$(AS) -c -o uldivmod.o uldivmod.s



####

disassem:	chiptune.elf
	$(OBJDUMP) --disassemble-all chiptune.elf > chiptune.dis

clean:	
	rm -f *~ *.o *.bin *.elf *.dis test_pt3
