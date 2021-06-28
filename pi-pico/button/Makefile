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
SRC_FILES = button.c

all:  button.uf2 button.dis

clean:
	rm -f button.bin button.elf *~ *.o *.dis *.uf2

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

button.elf: button.o \
	crt0.o bootrom.o runtime.o \
	timer.o clocks.o gpio.o idivmod.o uldivmod.o
	$(LD) -v -Tmemmap_default.ld -nostartfiles -o button.elf button.o \
		bootrom.o crt0.o runtime.o \
		timer.o clocks.o gpio.o idivmod.o uldivmod.o
#		boot2_w25q080.o 

bootrom.o:	bootrom.c
	$(CC) $(CFLAGS) -c bootrom.c

clocks.o:	clocks.c
	$(CC) $(CFLAGS) -c clocks.c

timer.o:	timer.c
	$(CC) $(CFLAGS) -c timer.c

button.o:	button.c
	$(CC) $(CFLAGS) -c button.c

button.uf2: button.elf
	$(ELF2UF2) button.elf button.uf2

button.dis:	button.elf
	$(OBJDUMP) -h button.elf > button.dis
	$(OBJDUMP) -d button.elf >> button.dis

####

gpio.o:	gpio.c ../include/gpio.h
	$(CC) $(CFLAGS) -c gpio.c


