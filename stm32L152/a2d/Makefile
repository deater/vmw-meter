-include ../Makefile.inc

all:	a2d_temp.bin

-include ../Makefile.lib

a2d_temp.bin:	a2d_temp.elf
	$(OBJCOPY) -Obinary a2d_temp.elf a2d_temp.bin

a2d_temp.elf:	a2d_temp.o ../lib/delay_lib.o ../lib/lcd_lib.o ../lib/num_to_string.o
	$(LD) -v $(LINKER_SCRIPT) -nostartfiles -o a2d_temp.elf a2d_temp.o ../lib/delay_lib.o ../lib/lcd_lib.o ../lib/num_to_string.o

a2d_temp.o:	a2d_temp.c
	$(CC) $(CFLAGS)  -c -o a2d_temp.o a2d_temp.c


flash:		a2d_temp.elf
#	openocd -f board/stm32ldiscovery.cfg -c "program filename.bin 0x08000000"
	openocd -f board/stm32ldiscovery.cfg \
			-c "program a2d_temp.elf verify" \
			-c "reset run"


disassem:	a2d_temp.elf
	$(OBJDUMP) --disassemble-all a2d_temp.elf > a2d_temp.dis

clean:	
	rm -f *~ *.o *.bin *.elf *.dis
