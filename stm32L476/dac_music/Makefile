CC = arm-none-eabi-gcc
LD = arm-none-eabi-ld
OBJCOPY = arm-none-eabi-objcopy

LINKER_SCRIPT = -TSTM32L476xG.ld

CFLAGS = -g -O2 -Wall
CFLAGS += -mlittle-endian -mthumb -mcpu=cortex-m4
CFLAGS += -fno-common

#CLIB = /usr/lib/arm-none-eabi/newlib/libc.a /usr/lib/gcc/arm-none-eabi/7.3.1/thumb/libgcc.a

all:	lab13.bin

-include ../Makefile.lib

lab13.bin:	lab13.elf
	$(OBJCOPY) -Obinary lab13.elf lab13.bin

lab13.elf:	main.o lcd.o
	$(LD) $(LINKER_SCRIPT) -nostartfiles -o lab13.elf main.o lcd.o

lcd.o:	lcd.c stm32l476xx.h
	$(CC) $(CFLAGS)  -c -o lcd.o lcd.c

main.o:	main.c stm32l476xx.h
	$(CC) $(CFLAGS)  -c -o main.o main.c


flash:		lab13.elf
#	openocd -f board/stm32ldiscovery.cfg -c "program filename.bin 0x08000000"
	openocd -f board/stm32l4discovery.cfg \
			-c "program lab13.elf verify" \
			-c "reset run"


disassem:	lab13.elf
	$(OBJDUMP) --disassemble-all lab13.elf > lab13.dis

clean:	
	rm -f *~ *.o *.bin *.elf *.dis
