CROSS = arm-none-eabi-
CC = gcc
AS = as
ASFLAGS = -mcpu=arm1176jzf-s
CFLAGS = -O2 -mcpu=arm1176jzf-s -marm -nostartfiles -ffreestanding -nostdlib
LFLAGS = --no-undefined -T $(LINKER_SCRIPT)
LINKER_SCRIPT = kernel.ld
