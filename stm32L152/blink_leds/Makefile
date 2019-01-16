-include ../Makefile.inc

all:	blink_led.bin

-include ../Makefile.lib

blink_led.bin:	blink_led.elf
	$(OBJCOPY) -Obinary blink_led.elf blink_led.bin

blink_led.elf:	blink_led.o ../lib/delay_lib.o
	$(LD) -v $(LINKER_SCRIPT) -nostartfiles -o blink_led.elf blink_led.o ../lib/delay_lib.o

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
