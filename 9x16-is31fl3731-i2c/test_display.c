/* Test the display
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

	/* configure */
	/* TODO: init all data to 0s */

	/* put to sleep */
	memset(buffer,0,14);
	buffer[0]=0;					// start register
	buffer[IS31FL3731_FUNC_SHUTDOWN+1]=1;		// 1 means wakeup
	result=write(i2c_fd,buffer,13+1);
	if ((result<0) || (result!=13+1)) {
		fprintf(stderr,"Error waking up\n");
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
	buffer[2]=0;					// start register
	buffer[1+0x00]=0xff;				// LEDs 0..7 on
	buffer[1+0x12]=0x00;				// blink off
	buffer[1+0x24]=0x80;				// LED0 brightness = 50%
	buffer[1+0x25]=0x70;				// LED1 brightness =
	buffer[1+0x26]=0x60;				// LED2 brightness =
	buffer[1+0x27]=0x50;				// LED3 brightness =
	buffer[1+0x28]=0x40;				// LED4 brightness =
	buffer[1+0x29]=0x30;				// LED5 brightness =
	buffer[1+0x2A]=0x20;				// LED6 brightness =
	buffer[1+0x2B]=0x10;				// LED7 brightness =

	result=write(i2c_fd,buffer,182);
	if ((result<0) || (result!=182)) {
		fprintf(stderr,"Error configuring\n");
	}



	close(i2c_fd);

	return 0;
}

