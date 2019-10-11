#include <stddef.h>
#include <stdint.h>
#include "printk.h"
#include "syscalls.h"
#include "console_io.h"
#include "timer.h"
#include "interrupts.h"
#include "bcm2835_periph.h"
#include "mmio.h"

extern int blinking_enabled;

/* Note!  Do not call a SWI from supervisor mode */
/* as the svc_lr and svc_spr can get corrupted   */

//uint32_t __attribute__((interrupt("SWI"))) swi_handler(

uint32_t swi_handler_c(
	uint32_t r0, uint32_t r1, uint32_t r2, uint32_t r3) {

	register long r7 asm ("r7");

	uint32_t result=0;
	char *ptr;

//	printk("Starting syscall %d\n",r7);

	switch(r7) {
		case SYSCALL_READ:
			ptr=(char *)r1;
//			printk("Trying to read: %d %x %d\n",r0,r1,r2);
			if (r0==0) {
				result=console_read(ptr,(size_t)r2);
			}
			else {
				printk("Attempting to read from unsupported fd %d\n",r0);
				result=-1;
			}
//			printk("Read %d bytes, %x\n",result,ptr[0]);
			break;

		case SYSCALL_WRITE:
			ptr=(char *)r1;
//			printk("Trying to write: %d %x(%x) %d\n",
//				r0,r1,ptr[0],r2);
			if ((r0==1) || (r0==2)) {
				result = console_write(ptr, (size_t)r2);
//				printk("After write, %d\n",result);
			}
			else {
				printk("Attempting to write unsupported fd %d\n",r0);
				result=-1;
			}
			break;

		case SYSCALL_TIME:
			{
				int *int_ptr;
				int_ptr=(int *)r0;
				*int_ptr=(tick_counter/64);
			}
			break;

		case SYSCALL_BLINK:
			if (r0==0) {
				printk("DISABLING BLINK\n");
				blinking_enabled=0;
			}
			else {
				printk("ENABLING_BLINK\n");
				blinking_enabled=1;
			}
			break;

		case SYSCALL_GRADIENT:
			framebuffer_gradient();
			break;

		default:
			printk("Unknown syscall %d\n",r7);
			break;
	}

	return result;

}

