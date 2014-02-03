CC = arm-linux-gnueabihf-gcc-4.7
LD = arm-linux-gnueabihf-ld
OBJCOPY = arm-linux-gnueabihf-objcopy
OBJDUMP = arm-linux-gnueabihf-objdump

# if we were using a STM32F4 board..
#CFLAGS = -Wall -I. -I../include -fno-common -O2 -g -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16

# But we're not, we have a STM32L
CFLAGS = -Wall -I. -I../include -fno-common -O2 -g -mcpu=cortex-m3 -mthumb

LINKER_SCRIPT = -T../common/STM32L152xB.ld

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
