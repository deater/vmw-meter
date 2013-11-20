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
	unsigned short display_buffer[DISPLAY_LINES];

	char *device="/dev/i2c-1";

	int i2c_fd,i;

	/* Init i2c */

	i2c_fd=init_i2c(device);
	if (i2c_fd < 0) {
		fprintf(stderr,"Error opening i2c dev file %s\n",device);
      		return -1;
	}

	/* Init nunchuck */

	if (init_nunchuck(i2c_fd)) {
		fprintf(stderr,"Error initializing nunchuck\n");
		return -1;
	}

	/* Init display */
	if (init_display(i2c_fd,HT16K33_ADDRESS1,15)) {
		fprintf(stderr,"Error opening display\n");
		return -1;
	}


	while(1) {

		/* Read Nunchuck */

		read_nunchuck(i2c_fd,&n_data);

/*		printf("Joystick X: %d\n",n_data.joy_x);
		printf("Joystick Y: %d\n",n_data.joy_y);
		printf("Accelerometer X: %d\n",n_data.acc_x);
		printf("Accelerometer Y: %d\n",n_data.acc_y);
		printf("Accelerometer Z: %d\n",n_data.acc_z);
		printf("Z button pressed: %d\n",n_data.z_pressed);
		printf("C button pressed: %d\n",n_data.c_pressed);
*/

		for(i=0;i<DISPLAY_LINES;i++) {
			display_buffer[i]=0;
		}

		if (n_data.c_pressed) {
			display_buffer[5]|=0x1e;
			display_buffer[4]|=0x12;
			display_buffer[3]|=0x12;
			display_buffer[2]|=0x12;
		}


		if (n_data.z_pressed) {
			display_buffer[5]|=0x12;
			display_buffer[4]|=0x1a;
			display_buffer[3]|=0x16;
			display_buffer[2]|=0x12;
		}


		if (n_data.joy_x>140) {
			display_buffer[0]|=0xc;
			display_buffer[1]|=0x12;
		}

		if (n_data.joy_x<100) {
			display_buffer[7]|=0xc;
			display_buffer[6]|=0x12;

		}


		if (n_data.joy_y>140) {
			display_buffer[5]|=0x1;
			display_buffer[4]|=0x80;
			display_buffer[3]|=0x80;
			display_buffer[2]|=0x1;
		}

		if (n_data.joy_y<100) {
			display_buffer[5]|=0x20;
			display_buffer[4]|=0x40;
			display_buffer[3]|=0x40;
			display_buffer[2]|=0x20;

		}

		if (n_data.acc_x<512) {
			if (n_data.acc_y<512) {
				display_buffer[7]|=0x80;
			}
			if (n_data.acc_y>512) {
				display_buffer[7]|=0x40;
			}
		}
		if (n_data.acc_x>512) {
			if (n_data.acc_y<512) {
				display_buffer[0]|=0x80;
			}
			if (n_data.acc_y>512) {
				display_buffer[0]|=0x40;
			}

		}


		/* Write Display */
		update_display(i2c_fd,HT16K33_ADDRESS1,display_buffer);



		usleep(100000);
	}

	return 0;

}
