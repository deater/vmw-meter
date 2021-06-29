#include <stdint.h>

#include "include/mmio.h"
#include "include/gpio.h"

#include "include/pads_bank0.h"
#include "include/io_bank0.h"


// from ./src/rp2_common/hardware_gpio/gpio.c


void gpio_set_function(uint32_t which, uint32_t function) {

//	if (which>=NUM_BANK0_GPIOS) {
//		printk("Error GPIO");
//	}

//	if (function << IO_BANK0_GPIO0_CTRL_FUNCSEL_LSB)&~IO_BANK0_GPIO_CTRL_FUNCSEL_BITS) {
//		printk("Error GPIO");
//	}


	/* Set input enable on, Output Disable Off */

	hw_write_masked(&padsbank0_hw->io[which],
		PADS_BANK0_GPIO0_IE_BITS,
		PADS_BANK0_GPIO0_IE_BITS | PADS_BANK0_GPIO0_OD_BITS );

	/* Zero all fields apart from fsel */
	/* We want this IO to do what the peripheral tells it. */
	/* This doesn't affect e.g. pullup/pulldown, */
	/* as these are in pad controls. */
	iobank0_hw->io[which].ctrl = function << IO_BANK0_GPIO0_CTRL_FUNCSEL_LSB;
}

// from

void gpio_init(uint32_t which) {
	sio_hw->gpio_oe_clr = 1ul << which;
	sio_hw->gpio_clr = 1ul << which;
	gpio_set_function(which, GPIO_FUNC_SIO);
}

// from ./src/rp2_common/hardware_gpio/include/hardware/gpio.h


static inline void gpio_set_dir_out_masked(uint32_t mask) {
	sio_hw->gpio_oe_set = mask;
}

static inline void gpio_set_dir_in_masked(uint32_t mask) {
	sio_hw->gpio_oe_clr = mask;
}

void gpio_set_dir(uint32_t which, uint32_t out) {

	uint32_t mask = 1ul << which;

	if (out) {
		gpio_set_dir_out_masked(mask);
	}
	else {
		gpio_set_dir_in_masked(mask);
	}
}

static void gpio_set_mask(uint32_t mask) {
	sio_hw->gpio_set = mask;
}


static void gpio_clr_mask(uint32_t mask) {
    sio_hw->gpio_clr = mask;
}

void gpio_put(uint32_t which, uint32_t value) {

	uint32_t mask = 1ul << which;

	if (value) {
		gpio_set_mask(mask);
	}
	else {
		gpio_clr_mask(mask);
	}
}


uint32_t gpio_get(uint32_t which) {
	return !!((1ul << which) & sio_hw->gpio_in);
}






// Note that, on RP2040, setting both pulls enables a "bus keep" function,
// i.e. weak pull to whatever is current high/low state of GPIO.
void gpio_set_pulls(uint32_t gpio, uint32_t up, uint32_t down) {

//invalid_params_if(GPIO, gpio >= NUM_BANK0_GPIOS);
    hw_write_masked(
            &padsbank0_hw->io[gpio],
            ( up << PADS_BANK0_GPIO0_PUE_LSB) |
		(down << PADS_BANK0_GPIO0_PDE_LSB),
            PADS_BANK0_GPIO0_PUE_BITS | PADS_BANK0_GPIO0_PDE_BITS
    );
}



void gpio_pull_up(uint32_t which) {
	gpio_set_pulls(which, 1, 0);
}
