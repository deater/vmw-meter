-include ../Makefile.inc

all:	lcd_count.bin

-include ../Makefile.lib

lcd_count.bin:	lcd_count.elf
	$(OBJCOPY) -Obinary lcd_count.elf lcd_count.bin

lcd_count.elf:	lcd_count.o ../lib/lcd_lib.o ../lib/delay_lib.o ../lib/num_to_string.o
	$(LD) $(LINKER_SCRIPT) -nostartfiles -o lcd_count.elf lcd_count.o ../lib/lcd_lib.o ../lib/delay_lib.o ../lib/num_to_string.o

lcd_count.o:	lcd_count.c
	$(CC) $(CFLAGS)  -c -o lcd_count.o lcd_count.c


flash:		lcd_count.elf
#	openocd -f board/stm32ldiscovery.cfg -c "program filename.bin 0x08000000"
	openocd -f board/stm32ldiscovery.cfg \
			-c "program lcd_count.elf verify" \
			-c "reset run"


disassem:	lcd_count.elf
	$(OBJDUMP) --disassemble-all lcd_count.elf > lcd_count.dis

clean:	
	rm -f *~ *.o *.bin *.elf *.dis
