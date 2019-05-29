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

all:	80mhz.bin

-include ../Makefile.lib

80mhz.bin:	80mhz.elf
	$(OBJCOPY) -Obinary 80mhz.elf 80mhz.bin

80mhz.elf:	80mhz.o lcd.o
	$(LD) $(LINKER_SCRIPT) -nostartfiles -o 80mhz.elf 80mhz.o lcd.o

####

lcd.o:	lcd.c stm32l476xx.h
	$(CC) $(CFLAGS)  -c -o lcd.o lcd.c

80mhz.o:	80mhz.c stm32l476xx.h
	$(CC) $(CFLAGS)  -c 80mhz.c

####

flash:		80mhz.elf
#	openocd -f board/stm32ldiscovery.cfg -c "program filename.bin 0x08000000"
	openocd -f board/stm32l4discovery.cfg \
			-c "program 80mhz.elf verify" \
			-c "reset run"


disassem:	80mhz.elf
	$(OBJDUMP) --disassemble-all 80mhz.elf > 80mhz.dis

clean:	
	rm -f *~ *.o *.bin *.elf *.dis test_pt3
