/*
 * gpio.h -- vmwOS driver for raspberry pi GPIO
 *	by Vince Weaver <vincent.weaver _at_ maine.edu>
 *
 *	Modeled on the Linux GPIO interface
 */

int gpio_request(int which_one, char *string);
int gpio_direction_input(int which_one);
int gpio_direction_output(int which_one);
int gpio_to_irq(int which_one);
int gpio_free(int which_one);
int gpio_get_value(int which_one);
int gpio_set_value(int which_one, int value);
int gpio_set_falling(int which_one);
int gpio_clear_interrupt(int which_one);
