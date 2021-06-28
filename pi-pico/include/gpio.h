/* see ./host/hardware_gpio/include/hardware/gpio.h */

#define GPIO_OUT 1
#define GPIO_IN	0

#define NUM_BANK0_GPIOS	30

void gpio_init(uint32_t which);
void gpio_set_dir(uint32_t which, uint32_t dir);
void gpio_put(uint32_t which, uint32_t value);
uint32_t gpio_get(uint32_t which);
