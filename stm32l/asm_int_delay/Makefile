-include ../Makefile.inc

all:	int_delay.bin

-include ../Makefile.lib

int_delay.bin:	int_delay.elf
	$(OBJCOPY) -Obinary int_delay.elf int_delay.bin

int_delay.elf:	int_delay.o
	$(LD) -v $(LINKER_SCRIPT) -nostartfiles -o int_delay.elf int_delay.o

int_delay.o:	int_delay.s
	$(AS) -o int_delay.o int_delay.s


flash:		int_delay.elf
#	openocd -f board/stm32ldiscovery.cfg -c "program filename.bin 0x08000000"
	openocd -f board/stm32ldiscovery.cfg \
			-c "program int_delay.elf verify" \
			-c "reset run"


disassem:	int_delay.elf
	$(OBJDUMP) --disassemble-all int_delay.elf > int_delay.dis

clean:	
	rm -f *~ *.o *.bin *.elf *.dis
