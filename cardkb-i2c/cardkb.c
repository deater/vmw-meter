/* Sample code for i2c cardkb keyboard */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <math.h>

#include <sys/time.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

#include "i2c_lib.h"

#include "cardkb.h"


int main(int argc, char **argv) {

	signed int result;

	int i2c_fd;
	unsigned char buffer[1];

	i2c_fd=init_i2c("/dev/i2c-1");
	if (i2c_fd < 0) {
		fprintf(stderr,"Error opening device!\n");
		return -1;
	}

	/* Init CardKB on i2c address 0x5f  */
	if (ioctl(i2c_fd, I2C_SLAVE, CARDKB_ADDR) < 0) {
		fprintf(stderr,"Error setting i2c address %x\n",
			CARDKB_ADDR);
		return -1;
	}

	while(1) {
		/* Read keypress */

	        if ( (read(i2c_fd, &buffer, 1)) !=1) {
        	        fprintf(stderr,"Error reading CardKB!\n");
                	return -1;
        	}

		printf("Read %x\n",buffer[0]);

		sleep(1);
	}

	return result;
}

