/* This is the standard ARM timer */
/* Described in Chapter 14 of the BCM2835 ARM Peripherals Document */
/* It is similar but not exactly the same as an ARM SP804 Timer */

/* The BCM2835 supports a number of other types of timer too */

/* Note that the timer we use is based on the APB clock which */
/* may change frequency in power-saving modes, so maybe not the */
/* best clock to be using */

#include <stdint.h>
#include "mmio.h"
#include "bcm2835_periph.h"
#include "led.h"
#include "time.h"

uint32_t tick_counter=0;

int timer_init(void) {

	uint32_t old;

	/* Disable the clock before changing config */
	old=mmio_read(TIMER_CONTROL);
	old&=~(TIMER_CONTROL_ENABLE|TIMER_CONTROL_INT_ENABLE);

	/* The value is loaded into TIMER_LOAD and then it counts down */
	/* and interrupts once it hits zero. */
	/* Then this value is automatically reloaded and restarted */


	/* Timer is based on the APB bus clock which is 250MHz on Rasp-Pi */

	/* First we scale this down to 1MHz using the pre-divider */
	/* We want to /250.  The pre-divider adds one, so 249 = 0xf9 */
	mmio_write(TIMER_PREDIVIDER,0xf9);

	/* We enable the /256 prescalar */
	/* So final frequency = 1MHz/256/X */

	/* 64 Hz */
	mmio_write(TIMER_LOAD,61);

	/* Enable the timer in 32-bit mode, enable interrupts */
	/* And pre-scale the clock down by 256 */
	mmio_write(TIMER_CONTROL,
		TIMER_CONTROL_32BIT |
		TIMER_CONTROL_ENABLE |
		TIMER_CONTROL_INT_ENABLE |
		TIMER_CONTROL_PRESCALE_256);

	/* Enable timer interrupt */
	mmio_write(IRQ_ENABLE_BASIC_IRQ,IRQ_ENABLE_BASIC_IRQ_ARM_TIMER);

	return 0;

}

uint32_t ticks_since_boot(void) {

	return tick_counter;

}

