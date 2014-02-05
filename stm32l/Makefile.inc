ifeq ($(ARCH),)
   ARCH = $(shell uname -m)
endif

ifneq (,$(findstring arm,$(ARCH)))

CC = gcc
LD = ld
OBJCOPY = objcopy
OBJDUMP = objdump

else

CC = arm-linux-gnueabihf-gcc-4.7
LD = arm-linux-gnueabihf-ld
OBJCOPY = arm-linux-gnueabihf-objcopy
OBJDUMP = arm-linux-gnueabihf-objdump

endif

# if we were using a STM32F4 board..
#CFLAGS = -Wall -I. -I../include -fno-common -O2 -g -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16

# But we're not, we have a STM32L
CFLAGS = -Wall -I. -I../include -fno-common -O2 -g -mcpu=cortex-m3 -mthumb
LINKER_SCRIPT = -T../common/STM32L152xB.ld

