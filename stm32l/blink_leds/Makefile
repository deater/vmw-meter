CC = arm-linux-gnueabihf-gcc-4.7
LD = arm-linux-gnueabihf-ld
OBJCOPY = arm-linux-gnueabihf-objcopy
OBJDUMP = arm-linux-gnueabihf-objdump

CFLAGS = -Wall -I. -I../include -fno-common -O2 -g -mcpu=cortex-m3 -mthumb

LINKER_SCRIPT = -T../common/STM32L152xB.ld

all:	blink_led.bin

blink_led.bin:	blink_led.elf
	$(OBJCOPY) -Obinary blink_led.elf blink_led.bin

blink_led.elf:	blink_led.o
	$(LD) -v $(LINKER_SCRIPT) -nostartfiles -o blink_led.elf blink_led.o

blink_led.o:	blink_led.c
	$(CC) $(CFLAGS)  -c -o blink_led.o blink_led.c


flash:		blink_led.elf
#	openocd -f board/stm32ldiscovery.cfg -c "program filename.bin 0x08000000"
	openocd -f board/stm32ldiscovery.cfg \
			-c "program blink_led.elf verify" \
			-c "reset run"


disassem:	blink_led.elf
	$(OBJDUMP) --disassemble-all blink_led.elf > blink_led.dis

clean:	
	rm -f *~ *.o *.bin *.elf *.dis
