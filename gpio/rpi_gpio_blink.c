/* Blink an LED on the rasp-pi */

#include <stdio.h>
#include <unistd.h>

#include "gpio_lib.h"

int main(int argc, char **argv) {

	int value1,value2;

	printf("RPI GPIO test\n");

	gpio_enable(4);

	gpio_set_write(4);

	value1=gpio_read(4);

	value2=!value1;

	while(1){
		gpio_write(4,value1);

		value1=!value1;
		value2=!value2;
		usleep(500000);

	}

	return 0;
}
