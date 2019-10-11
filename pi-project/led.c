#include <stdint.h>
#include "bcm2835_periph.h"
#include "led.h"
#include "mmio.h"
#include "hardware.h"
#include "gpio.h"
#include "printk.h"

/* Default for Model B */
static uint32_t led_gpio=16;
static uint32_t led_active_low=1;

/* Enable GPIO for the ACT LED */
int led_init(void) {

	/* On the Model B this is 16, on the B+/A+ this is 47 */
	/* Model B is active low, B+/A+ active high */
	if ((hardware_type==RPI_MODEL_BPLUS) ||
	 	(hardware_type==RPI_MODEL_APLUS)) {
		led_gpio=47;
		led_active_low=0;
	}

	gpio_request(led_gpio,"act_led");
	gpio_direction_output(led_gpio);

	printk("Starting heartbeat LED on GPIO%d\n",led_gpio);

	return 0;

}

int led_on(void) {

	if (led_active_low) {
		gpio_set_value(led_gpio,0);
	}
	else {
		gpio_set_value(led_gpio,1);
	}

	return 0;
}

int led_off(void) {

	if (led_active_low) {
		gpio_set_value(led_gpio,1);
	}
	else {
		gpio_set_value(led_gpio,0);
	}

	return 0;
}

