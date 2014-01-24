CC = arm-linux-gnueabihf-gcc-4.7
LD = arm-linux-gnueabihf-ld
OBJCOPY = arm-linux-gnueabihf-objcopy
OBJDUMP = arm-linux-gnueabihf-objdump

CFLAGS = -Wall -I. -I../include -fno-common -O0 -g -mcpu=cortex-m3 -mthumb

LINKER_SCRIPT = -T../common/STM32L152xB.ld

all:	lcd_display.bin

lcd_display.bin:	lcd_display.elf
	$(OBJCOPY) -Obinary lcd_display.elf lcd_display.bin

lcd_display.elf:	lcd_display.o
	$(LD) -v $(LINKER_SCRIPT) -nostartfiles -o lcd_display.elf lcd_display.o

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
