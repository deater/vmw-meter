include Makefile.inc

all:	kernel.img kernel.dis

kernel.img:	kernel.elf
	$(CROSS)objcopy kernel.elf -O binary kernel.img



kernel.elf:	kernel_main.o \
		atags.o boot.o interrupts.o \
		console_io.o framebuffer.o framebuffer_console.o gpio.o led.o \
		mailbox.o memory.o mmu.o printk.o \
		serial.o string.o swi_handler.o syscalls.o timer.o \
		./user/shell.o ./user/vlibc.o
	$(CROSS)ld $(LFLAGS) \
		kernel_main.o \
		atags.o boot.o interrupts.o \
		console_io.o framebuffer.o framebuffer_console.o gpio.o led.o \
		mailbox.o memory.o mmu.o printk.o \
		serial.o string.o swi_handler.o syscalls.o timer.o \
		./user/shell.o ./user/vlibc.o \
		-Map kernel.map -o kernel.elf


kernel_main.o:	kernel_main.c
	$(CROSS)$(CC) $(CFLAGS) -o kernel_main.o -c kernel_main.c

user/shell.o:	force_look
	cd user && make

user/vlibc.o:	force_look
	cd user && make


atags.o:	atags.c atags.h
	$(CROSS)$(CC) $(CFLAGS) -o atags.o -c atags.c

console_io.o:	console_io.c console_io.h
	$(CROSS)$(CC) $(CFLAGS) -o console_io.o -c console_io.c

framebuffer.o:	framebuffer.c framebuffer.h
	$(CROSS)$(CC) $(CFLAGS) -o framebuffer.o -c framebuffer.c

framebuffer_console.o:	framebuffer_console.c framebuffer_console.h
	$(CROSS)$(CC) $(CFLAGS) -o framebuffer_console.o -c framebuffer_console.c

gpio.o:	gpio.c gpio.h
	$(CROSS)$(CC) $(CFLAGS) -o gpio.o -c gpio.c

interrupts.o:	interrupts.c
	$(CROSS)$(CC) $(CFLAGS) -o interrupts.o -c interrupts.c

led.o:	led.c led.h
	$(CROSS)$(CC) $(CFLAGS) -o led.o -c led.c

mailbox.o:	mailbox.c mailbox.h
	$(CROSS)$(CC) $(CFLAGS) -o mailbox.o -c mailbox.c

memory.o:	memory.c memory.h
	$(CROSS)$(CC) $(CFLAGS) -o memory.o -c memory.c

mmu.o:	mmu.c mmu.h
	$(CROSS)$(CC) $(CFLAGS) -o mmu.o -c mmu.c

printk.o:	printk.c printk.h
	$(CROSS)$(CC) $(CFLAGS) -o printk.o -c printk.c

serial.o:	serial.c bcm2835_periph.h serial.h
	$(CROSS)$(CC) $(CFLAGS) -o serial.o -c serial.c

string.o:	string.c string.h
	$(CROSS)$(CC) $(CFLAGS) -o string.o -c string.c

syscalls.o:	syscalls.c syscalls.h
	$(CROSS)$(CC) $(CFLAGS) -o syscalls.o -c syscalls.c

timer.o:	timer.c timer.h
	$(CROSS)$(CC) $(CFLAGS) -o timer.o -c timer.c



boot.o:	boot.s
	$(CROSS)as $(ASFLAGS) -o boot.o boot.s

swi_handler.o:	swi_handler.s
	$(CROSS)as $(ASFLAGS) -o swi_handler.o swi_handler.s


kernel.dis:	kernel.elf
	$(CROSS)objdump --disassemble-all kernel.elf > kernel.dis

clean:
	rm -f *~ *.o *.map *.elf kernel.img *.dis
	cd user && make clean

submit:
	tar -czvf hw9_submit.tar.gz *.c *.h *.s *.ld Makefile Makefile.inc README user


force_look:
	true
