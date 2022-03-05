#include <stdint.h>

#include "include/pi-pico.h"
#include "include/gpio.h"
#include "include/timer.h"

int main(int argc, char **argv) {

	const uint32_t LED_PIN = 2;
	const uint32_t BUTTON_PIN = 3;

	uint32_t old_state=0,state=0;
	uint32_t blinking=1;

	gpio_init(LED_PIN);
	gpio_set_dir(LED_PIN, GPIO_OUT);

	gpio_init(BUTTON_PIN);
	gpio_set_dir(BUTTON_PIN, GPIO_IN);

	while (1) {
		if (blinking) {
			gpio_put(LED_PIN, 1);
			sleep_ms(250);
			gpio_put(LED_PIN, 0);
			sleep_ms(250);
		}

		state=gpio_get(BUTTON_PIN);
		if (state!=old_state) {
			if (state==0) {
				blinking=!blinking;
			}
			old_state=state;
		}

	}

	return 0;

}
