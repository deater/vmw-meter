#include <stdint.h>

#include "include/pi-pico.h"
#include "include/gpio.h"
#include "include/timer.h"

int main(int argc, char **argv) {

	const uint32_t LED_PIN = PICO_DEFAULT_LED_PIN;

	gpio_init(LED_PIN);
	gpio_set_dir(LED_PIN, GPIO_OUT);

	while (1) {
		gpio_put(LED_PIN, 1);
		sleep_ms(250);
		gpio_put(LED_PIN, 0);
		sleep_ms(250);
	}

	return 0;

}
