CC = arm-linux-gnueabihf-gcc-4.7
LD = arm-linux-gnueabihf-ld
OBJCOPY = arm-linux-gnueabihf-objcopy
OBJDUMP = arm-linux-gnueabihf-objdump

CFLAGS = -Wall -I. -I../include -fno-common -O2 -g -mcpu=cortex-m3 -mthumb

LINKER_SCRIPT = -T../common/STM32L152xB.ld

all:	push_button.bin

push_button.bin:	push_button.elf
	$(OBJCOPY) -Obinary push_button.elf push_button.bin

push_button.elf:	push_button.o
	$(LD) -v $(LINKER_SCRIPT) -nostartfiles -o push_button.elf push_button.o

push_button.o:	push_button.c
	$(CC) $(CFLAGS)  -c -o push_button.o push_button.c


flash:		push_button.elf
#	openocd -f board/stm32ldiscovery.cfg -c "program filename.bin 0x08000000"
	openocd -f board/stm32ldiscovery.cfg \
			-c "program push_button.elf verify" \
			-c "reset run"


disassem:	push_button.elf
	$(OBJDUMP) --disassemble-all push_button.elf > push_button.dis

clean:	
	rm -f *~ *.o *.bin *.elf *.dis
