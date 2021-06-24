CC = arm-none-eabi-gcc
AS = arm-none-eabi-as
LD = arm-none-eabi-ld

OBJDUMP = arm-none-eabi-objdump
OBJCOPY = arm-none-eabi-objcopy
#CFLAGS = -g -O2 -Wall
CFLAGS = -g -O3 -Wall -I..
CFLAGS += -mlittle-endian -mthumb -mcpu=cortex-m0plus
CFLAGS += -fno-common
CFLAGS += -ffunction-sections -fdata-sections
ASFLAGS =

ELF2UF2	= ../elf2uf2/elf2uf2

##### Project specific libraries #####
SRC_FILES = blink.c

all:  blink.uf2 blink.dis

clean:
	rm -f blink.bin blink.elf *~ *.o *.dis *.uf2

crt0.o:	crt0.S
	$(AS) $(ASFLAGS) -o crt0.o -c crt0.S

#boot2_w25q080.o:	boot2_w25q080.S
#	$(AS) $(ASFLAGS) -o boot2_w25q080.o -c boot2_w25q080.S

idivmod.o:	idivmod.S
	$(AS) $(ASFLAGS) -o idivmod.o -c idivmod.S

uldivmod.o:	uldivmod.S
	$(AS) $(ASFLAGS) -o uldivmod.o -c uldivmod.S

runtime.o:	runtime.c
	$(CC) $(CFLAGS) -c runtime.c

blink.elf: blink.o crt0.o bootrom.o runtime.o clocks.o gpio.o idivmod.o uldivmod.o
	$(LD) -v -Tmemmap_default.ld -nostartfiles -o blink.elf blink.o \
		bootrom.o crt0.o \
		runtime.o clocks.o gpio.o idivmod.o uldivmod.o
#		boot2_w25q080.o 

bootrom.o:	bootrom.c
	$(CC) $(CFLAGS) -c bootrom.c

clocks.o:	clocks.c
	$(CC) $(CFLAGS) -c clocks.c

blink.o:	blink.c
	$(CC) $(CFLAGS) -c blink.c

blink.uf2: blink.elf
	$(ELF2UF2) blink.elf blink.uf2

blink.dis:	blink.elf
	$(OBJDUMP) -h blink.elf > blink.dis
	$(OBJDUMP) -d blink.elf >> blink.dis

####

gpio.o:	gpio.c ../include/gpio.h
	$(CC) $(CFLAGS) -c gpio.c


