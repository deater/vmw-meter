-include ../Makefile.inc

all:	range_finder.bin

-include ../Makefile.lib

range_finder.bin:	range_finder.elf
	$(OBJCOPY) -Obinary range_finder.elf range_finder.bin

range_finder.elf:	range_finder.o ../lib/delay_lib.o ../lib/lcd_lib.o ../lib/num_to_string.o
	$(LD) -v $(LINKER_SCRIPT) -nostartfiles -o range_finder.elf range_finder.o ../lib/delay_lib.o ../lib/lcd_lib.o ../lib/num_to_string.o

range_finder.o:	range_finder.c
	$(CC) $(CFLAGS)  -c -o range_finder.o range_finder.c


flash:		range_finder.elf
#	openocd -f board/stm32ldiscovery.cfg -c "program filename.bin 0x08000000"
	openocd -f board/stm32ldiscovery.cfg \
			-c "program range_finder.elf verify" \
			-c "reset run"


disassem:	range_finder.elf
	$(OBJDUMP) --disassemble-all range_finder.elf > range_finder.dis

clean:	
	rm -f *~ *.o *.bin *.elf *.dis
