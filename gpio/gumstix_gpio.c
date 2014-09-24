/* Test the GPIO interface on gumstix board */

#include <stdio.h>
#include <unistd.h>

#include "gpio_lib.h"

int main(int argc, char **argv) {

	int value1,value2;

	printf("Gumstix GPIO test\n");

	gpio_enable(146);
	gpio_enable(147);

	gpio_set_write(146);
	gpio_set_write(147);

	value1=gpio_read(146);
	value2=gpio_read(147);

	value2=!value1;

	while(1){
		gpio_write(146,value1);
		gpio_write(147,value2);

		value1=!value1;
		value2=!value2;
		usleep(500000);

	}

	return 0;
}
