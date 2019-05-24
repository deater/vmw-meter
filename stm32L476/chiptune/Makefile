CC = arm-none-eabi-gcc
LD = arm-none-eabi-ld
OBJCOPY = arm-none-eabi-objcopy

LINKER_SCRIPT = -TSTM32L476xG.ld

CFLAGS = -g -O2 -Wall
CFLAGS += -mlittle-endian -mthumb -mcpu=cortex-m4
CFLAGS += -fno-common

#CLIB = /usr/lib/arm-none-eabi/newlib/libc.a /usr/lib/gcc/arm-none-eabi/7.3.1/thumb/libgcc.a

all:	chiptune.bin

-include ../Makefile.lib

chiptune.bin:	chiptune.elf
	$(OBJCOPY) -Obinary chiptune.elf chiptune.bin

chiptune.elf:	main.o lcd.o pt3_lib.o string.o
	$(LD) $(LINKER_SCRIPT) -nostartfiles -o chiptune.elf main.o lcd.o pt3_lib.o string.o

####

string.o:	string.c string.h
	$(CC) $(CFLAGS)  -c -o string.o string.c

pt3_lib.o:	pt3_lib.c pt3_lib.h
	$(CC) $(CFLAGS)  -c -o pt3_lib.o pt3_lib.c

lcd.o:	lcd.c stm32l476xx.h
	$(CC) $(CFLAGS)  -c -o lcd.o lcd.c

main.o:	main.c stm32l476xx.h
	$(CC) $(CFLAGS)  -c -o main.o main.c



####

flash:		chiptune.elf
#	openocd -f board/stm32ldiscovery.cfg -c "program filename.bin 0x08000000"
	openocd -f board/stm32l4discovery.cfg \
			-c "program chiptune.elf verify" \
			-c "reset run"


disassem:	chiptune.elf
	$(OBJDUMP) --disassemble-all chiptune.elf > chiptune.dis

clean:	
	rm -f *~ *.o *.bin *.elf *.dis
