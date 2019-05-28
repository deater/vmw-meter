AS = arm-none-eabi-as
CC = arm-none-eabi-gcc
LD = arm-none-eabi-ld
OBJCOPY = arm-none-eabi-objcopy

LINKER_SCRIPT = -TSTM32L476xG.ld

CFLAGS = -g -O2 -Wall
CFLAGS += -mlittle-endian -mthumb -mcpu=cortex-m4
CFLAGS += -fno-common

#CFLAGS_NATIVE = -g -O2 -Wall -m32
#LFLAGS_NATIVE = -m32

CFLAGS_NATIVE = -g -O2 -Wall
LFLAGS_NATIVE =

#CLIB = /usr/lib/arm-none-eabi/newlib/libc.a /usr/lib/gcc/arm-none-eabi/7.3.1/thumb/libgcc.a

CC_NATIVE = gcc

all:	chiptune.bin test_pt3

-include ../Makefile.lib

chiptune.bin:	chiptune.elf
	$(OBJCOPY) -Obinary chiptune.elf chiptune.bin

chiptune.elf:	chiptune.o i2c.o lcd.o pt3_lib.o string.o ay8912.o ldivmod.o uldivmod.o
	$(LD) $(LINKER_SCRIPT) -nostartfiles -o chiptune.elf chiptune.o i2c.o lcd.o pt3_lib.o string.o ay8912.o ldivmod.o uldivmod.o

####

test_pt3:	test_pt3.o ay8912_native.o pt3_lib_native.o
	$(CC_NATIVE) $(LFLAGS_NATIVE) -o test_pt3 test_pt3.o ay8912_native.o pt3_lib_native.o

test_pt3.o:	test_pt3.c
	$(CC_NATIVE) $(CFLAGS_NATIVE) -c test_pt3.c

ay8912_native.o:	ay8912.c
	$(CC_NATIVE) $(CFLAGS_NATIVE) -o ay8912_native.o -c ay8912.c

pt3_lib_native.o:	pt3_lib.c
	$(CC_NATIVE) $(CFLAGS_NATIVE) -o pt3_lib_native.o -c pt3_lib.c

####

ldivmod.o:	ldivmod.s
	$(AS) -c -o ldivmod.o ldivmod.s

uldivmod.o:	uldivmod.s
	$(AS) -c -o uldivmod.o uldivmod.s

####

string.o:	string.c string.h
	$(CC) $(CFLAGS)  -c -o string.o string.c

ay8192.o:	ay8912.c ay8912.h
	$(CC) $(CFLAGS)  -c -o ay8912.o ay8912.c

pt3_lib.o:	pt3_lib.c pt3_lib.h
	$(CC) $(CFLAGS)  -c -o pt3_lib.o pt3_lib.c

lcd.o:	lcd.c stm32l476xx.h
	$(CC) $(CFLAGS)  -c -o lcd.o lcd.c

i2c.o:	i2c.c stm32l476xx.h
	$(CC) $(CFLAGS)  -c -o i2c.o i2c.c

chiptune.o:	chiptune.c stm32l476xx.h
	$(CC) $(CFLAGS)  -c -o chiptune.o chiptune.c



####

flash:		chiptune.elf
#	openocd -f board/stm32ldiscovery.cfg -c "program filename.bin 0x08000000"
	openocd -f board/stm32l4discovery.cfg \
			-c "program chiptune.elf verify" \
			-c "reset run"


disassem:	chiptune.elf
	$(OBJDUMP) --disassemble-all chiptune.elf > chiptune.dis

clean:	
	rm -f *~ *.o *.bin *.elf *.dis test_pt3
