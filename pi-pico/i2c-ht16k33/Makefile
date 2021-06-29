CC = arm-none-eabi-gcc
AS = arm-none-eabi-as
LD = arm-none-eabi-ld

OBJDUMP = arm-none-eabi-objdump
OBJCOPY = arm-none-eabi-objcopy
#CFLAGS = -g -O2 -Wall
CFLAGS = -g -O3 -Wall -I..
CFLAGS += -mlittle-endian -mthumb -mcpu=cortex-m0plus
CFLAGS += -fno-common -fno-builtin
CFLAGS += -ffunction-sections -fdata-sections
ASFLAGS =

ELF2UF2	= ../elf2uf2/elf2uf2

##### Project specific libraries #####
SRC_FILES = i2c-display.c

all:  i2c-display.uf2 i2c-display.dis

clean:
	rm -f i2c-display.bin i2c-display.elf *~ *.o *.dis *.uf2

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

i2c-display.elf: i2c-display.o \
	crt0.o bootrom.o runtime.o \
	mmio.o timer.o clocks.o gpio.o i2c.o idivmod.o uldivmod.o \
	write_display.o
	$(LD) -v -Tmemmap_default.ld -nostartfiles -o i2c-display.elf i2c-display.o \
		bootrom.o crt0.o runtime.o \
		mmio.o timer.o clocks.o gpio.o i2c.o idivmod.o uldivmod.o \
		write_display.o
#		boot2_w25q080.o

bootrom.o:	bootrom.c
	$(CC) $(CFLAGS) -c bootrom.c

clocks.o:	clocks.c
	$(CC) $(CFLAGS) -c clocks.c

timer.o:	timer.c
	$(CC) $(CFLAGS) -c timer.c

i2c-display.o:	i2c-display.c
	$(CC) $(CFLAGS) -c i2c-display.c

i2c-display.uf2: i2c-display.elf
	$(ELF2UF2) i2c-display.elf i2c-display.uf2

i2c-display.dis:	i2c-display.elf
	$(OBJDUMP) -h i2c-display.elf > i2c-display.dis
	$(OBJDUMP) -d i2c-display.elf >> i2c-display.dis

####

gpio.o:	gpio.c ../include/gpio.h
	$(CC) $(CFLAGS) -c gpio.c

####

i2c.o:	i2c.c ./i2c.h
	$(CC) $(CFLAGS) -c i2c.c

####

mmio.o:	mmio.c
	$(CC) $(CFLAGS) -c mmio.c

####

write_display.o:	write_display.c write_display.h
	$(CC) $(CFLAGS) -c write_display.c

