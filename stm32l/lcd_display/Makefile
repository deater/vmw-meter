-include ../Makefile.inc

all:	lcd_display.bin

-include ../Makefile.lib

lcd_display.bin:	lcd_display.elf
	$(OBJCOPY) -Obinary lcd_display.elf lcd_display.bin

lcd_display.elf:	lcd_display.o ../lib/lcd_lib.o ../lib/delay_lib.o
	$(LD) -v $(LINKER_SCRIPT) -nostartfiles -o lcd_display.elf lcd_display.o ../lib/lcd_lib.o ../lib/delay_lib.o

lcd_display.o:	lcd_display.c
	$(CC) $(CFLAGS)  -c -o lcd_display.o lcd_display.c


flash:		lcd_display.elf
#	openocd -f board/stm32ldiscovery.cfg -c "program filename.bin 0x08000000"
	openocd -f board/stm32ldiscovery.cfg \
			-c "program lcd_display.elf verify" \
			-c "reset run"


disassem:	lcd_display.elf
	$(OBJDUMP) --disassemble-all lcd_display.elf > lcd_display.dis

clean:	
	rm -f *~ *.o *.bin *.elf *.dis
