-include ../Makefile.inc

all:	pwm_led.bin

-include ../Makefile.lib

pwm_led.bin:	pwm_led.elf
	$(OBJCOPY) -Obinary pwm_led.elf pwm_led.bin

pwm_led.elf:	pwm_led.o
	$(LD) -v $(LINKER_SCRIPT) -nostartfiles -o pwm_led.elf pwm_led.o

pwm_led.o:	pwm_led.s
	$(AS) -o pwm_led.o pwm_led.s


flash:		pwm_led.elf
#	openocd -f board/stm32ldiscovery.cfg -c "program filename.bin 0x08000000"
	openocd -f board/stm32ldiscovery.cfg \
			-c "program pwm_led.elf verify" \
			-c "reset run"


disassem:	pwm_led.elf
	$(OBJDUMP) --disassemble-all pwm_led.elf > pwm_led.dis

clean:	
	rm -f *~ *.o *.bin *.elf *.dis
