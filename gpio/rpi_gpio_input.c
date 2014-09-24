/* Read the status of GPIO rasp-pi */

#include <stdio.h>
#include <unistd.h>

#include "gpio_lib.h"

int main(int argc, char **argv) {

	int value1;
	int gpio_num=18;

	printf("Read RPI GPIO %d\n",gpio_num);

	gpio_enable(gpio_num);

	gpio_set_read(gpio_num);


	while(1){
		value1=gpio_read(gpio_num);

		printf("Value: %d\n",value1);
		usleep(500000);

	}

	return 0;
}
