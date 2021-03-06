#include <stdint.h>

#include "include/mmio.h"
#include "include/pi-pico.h"
#include "include/gpio.h"
#include "include/timer.h"

#include "include/i2c.h"
#include "i2c.h"

#include "write_display.h"

/* ./src/rp2040/hardware_structs/include/hardware/structs/i2c.h */
//#define PICO_DEFAULT_I2C 0
//#define PICO_DEFAULT_I2C_INSTANCE (__CONCAT(i2c,PICO_DEFAULT_I2C))
//#define i2c_default PICO_DEFAULT_I2C_INSTANCE

struct i2c_inst_t i2c0 = {i2c0_hw, 0};

int main(int argc, char **argv) {

	uint32_t count=0;

	const uint32_t LED_PIN = 2;

	/* Init i2c */
	i2c_init(&i2c0, 100 * 1000);
	gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
	gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
	gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);
	gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);
	init_display(&i2c0);

	/* Init LED for Blinking */
	gpio_init(LED_PIN);
	gpio_set_dir(LED_PIN, GPIO_OUT);

	while (1) {
		gpio_put(LED_PIN, 1);
		sleep_ms(250);
		gpio_put(LED_PIN, 0);
		sleep_ms(250);

		write_display(&i2c0,count);
		count++;
	}

	return 0;

}
