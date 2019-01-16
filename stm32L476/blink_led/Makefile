CC = arm-none-eabi-gcc
LD = arm-none-eabi-ld

OBJCOPY = arm-none-eabi-objcopy
CFLAGS = -g -O2 -Wall
CFLAGS += -mlittle-endian -mthumb -mcpu=cortex-m4
CFLAGS += -fno-common

##### Project specific libraries #####
SRC_FILES = blinky.c startup_stm32l476xx.s system_stm32l4xx.c

all:  blinky.bin

clean:
	rm -f blinky.bin blinky.elf *~ *.o

blinky.elf: blinky.o
	$(LD) -v -TSTM32L476xG.ld -nostartfiles -o blinky.elf blinky.o

blinky.o:	blinky.c
	$(CC) $(CFLAGS) -c blinky.c

blinky.bin: blinky.elf
	$(OBJCOPY) -O binary $^ $@

