#include <stdint.h>
#include "bcm2835_periph.h"
#include "mmio.h"
#include "led.h"
#include "timer.h"

/* global variable */
int blinking_enabled=1;


void __attribute__((interrupt("IRQ"))) interrupt_handler(void) {

	static int lit = 0;
	uint32_t pending;
	uint32_t entry_lr;

        pending=mmio_read(IRQ_BASIC_PENDING);

	/* Check that it was in fact a timer interrupt */
        if ((pending & 0x1)!=0x1) {
		printk("Unknown interrupt %x\n",pending);
        }

	else {
		/* Clear the ARM Timer interrupt		*/
		mmio_write(TIMER_IRQ_CLEAR,0x1);
		tick_counter++;

		if (!blinking_enabled) return;
		lit++;

		/* Flip the LED */
		if( lit==32 ) {
			led_off();
		}
		else if (lit==64) {
			led_on();
			lit = 0;
		}


	}
}

void __attribute__((interrupt("UNDEF"))) undefined_handler(void) {

	printk("Undefined instruction!\n");
	while(1);

}

void __attribute__((interrupt("ABORT"))) prefetch_handler(void) {

	printk("Prefetch handler!\n");
	while(1);

}

void __attribute__((interrupt("ABORT"))) data_handler(void) {

	printk("Data handler!\n");
	while(1);

}

void __attribute__((interrupt("FIQ"))) fiq_handler(void) {

	printk("FIQ handler!\n");
	while(1);

}

void __attribute__((interrupt("SVC"))) reset_error(void) {

	printk("Reset triggered!\n");
	while(1);

}


