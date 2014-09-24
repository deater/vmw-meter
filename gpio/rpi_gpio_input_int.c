/* Read a GPIO using an interrupt */

#include <stdio.h>
#include <unistd.h>
#include <poll.h>

#include <fcntl.h>

#include "gpio_lib.h"

int main(int argc, char **argv) {

	int value1;
	int gpio_num=18;
	int fd;

	printf("Read RPI GPIO %d\n",gpio_num);

	gpio_enable(gpio_num);
	gpio_enable(4);

	gpio_set_read(gpio_num);
	gpio_set_write(4);

	value1=0;
	gpio_write(4,value1);

	FILE *fff;
	fff=fopen("/sys/class/gpio/gpio18/edge","w");
	fprintf(fff,"rising");
	fclose(fff);

	fd=open("/sys/class/gpio/gpio18/value",O_RDONLY);

	struct pollfd fds;
	int result;

	fds.fd=fd;
	fds.events=POLLPRI|POLLERR;

	char buffer[10];

	while(1) {
		result=poll(&fds,1, -1);

		if (result<0) {
			printf("Error!\n");
		}
		else {
			printf("\t%x\n",fds.revents);
		}

		lseek(fd,0,SEEK_SET);
		read(fd,buffer,1);

		value1=!value1;
		gpio_write(4,value1);

		printf("Value: %c\n",buffer[0]);
	}
	return 0;
}
