/* Disable a 19x6 is31fl3731 display
*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdint.h>
#include <string.h>

#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

#include "is31fl3731.h"

int main(int argc, char **argv) {

	int result;
	int i2c_fd;
	uint8_t buffer[256+2];

	i2c_fd=open("/dev/i2c-1",O_RDWR);
	if (i2c_fd < 0) {
		fprintf(stderr,"Error opening device!\n");
		return -1;
	}

	if (ioctl(i2c_fd, I2C_SLAVE, IS31FL3731_ADDR)<0) {
		fprintf(stderr,"Error setting i2c address 0x%x\n",IS31FL3731_ADDR);

		return -1;
	}

	/* Initialize */

	/* set bank */
	memset(buffer,0,3);
	buffer[0]=IS31FL3731_COMMAND_REGISTER;
	buffer[1]=IS31FL3731_CMD_FUNCTION;		// function register
	result=write(i2c_fd,buffer,2);
	if ((result<0) || (result!=2)) {
		fprintf(stderr,"Error setting command bank\n");
	}

	/* put to sleep */
	memset(buffer,0,14);
	buffer[0]=0;					// start register
	buffer[IS31FL3731_FUNC_SHUTDOWN+1]=0;		// 0 means shutdown
	result=write(i2c_fd,buffer,13+1);
	if ((result<0) || (result!=13+1)) {
		fprintf(stderr,"Error putting to sleep\n");
	}

	/* set bank */
	memset(buffer,0,3);
	buffer[0]=IS31FL3731_COMMAND_REGISTER;
	buffer[1]=IS31FL3731_CMD_PAGE1;			// page1
	result=write(i2c_fd,buffer,2);
	if ((result<0) || (result!=2)) {
		fprintf(stderr,"Error setting page1\n");
	}


	/* whole screen half brightness */
	memset(buffer,0,182);
	buffer[1]=0;					// start register

	result=write(i2c_fd,buffer,182);
	if ((result<0) || (result!=182)) {
		fprintf(stderr,"Error configuring\n");
	}



	close(i2c_fd);

	return 0;
}

