/* Makes a spinning pattern on the display */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include <errno.h>

#include <sys/ioctl.h>

#include <linux/i2c-dev.h>

#include "i2c_lib.h"


int main(int arg, char **argv) {

	struct nunchuck_data n_data;

	char *device="/dev/i2c-1";

	int i2c_fd;

	i2c_fd=init_i2c(device);
	if (i2c_fd < 0) {
		fprintf(stderr,"Error opening i2c dev file %s\n",device);
      		return -1;
	}

	if (init_nunchuck(i2c_fd)) {
		fprintf(stderr,"Error initializing nunchuck\n");
		return -1;
	}


	while(1) {

		read_nunchuck(i2c_fd,&n_data);

		printf("Joystick X: %d\n",n_data.joy_x);
		printf("Joystick Y: %d\n",n_data.joy_y);
		printf("Accelerometer X: %d\n",n_data.acc_x);
		printf("Accelerometer Y: %d\n",n_data.acc_y);
		printf("Accelerometer Z: %d\n",n_data.acc_z);
		printf("Z button pressed: %d\n",n_data.z_pressed);
		printf("C button pressed: %d\n",n_data.c_pressed);

		usleep(100000);
	}

	return 0;

}
