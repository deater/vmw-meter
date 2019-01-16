-include ../Makefile.inc

all:	pwm_blink.bin

pwm_blink.bin:	pwm_blink.elf
	$(OBJCOPY) -Obinary pwm_blink.elf pwm_blink.bin

pwm_blink.elf:	pwm_blink.o
	$(LD) -v $(LINKER_SCRIPT) -nostartfiles -o pwm_blink.elf pwm_blink.o

pwm_blink.o:	pwm_blink.c
	$(CC) $(CFLAGS)  -c -o pwm_blink.o pwm_blink.c


flash:		pwm_blink.elf
#	openocd -f board/stm32ldiscovery.cfg -c "program filename.bin 0x08000000"
	openocd -f board/stm32ldiscovery.cfg \
			-c "program pwm_blink.elf verify" \
			-c "reset run"


disassem:	pwm_blink.elf
	$(OBJDUMP) --disassemble-all pwm_blink.elf > pwm_blink.dis

clean:	
	rm -f *~ *.o *.bin *.elf *.dis
