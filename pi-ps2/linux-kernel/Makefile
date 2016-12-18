ifneq ($(KERNELRELEASE),)
	
# kbuild part of makefile
obj-m  := pi-ps2gpio.o

else
	
# normal makefile
KDIR ?= /lib/modules/`uname -r`/build

	
default:
	$(MAKE) -C $(KDIR) M=$$PWD

	# Module specific targets

endif

clean:
	rm -f *~ *.o *.ko modules.order Module.symvers pi-ps2gpio.mod.c

