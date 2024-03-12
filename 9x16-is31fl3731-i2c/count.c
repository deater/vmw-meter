/* Scanner
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

	/* there are 8 frames */

	/* 18 bytes each frame */
	/* so 18*8 = 144 LEDs */
	/* 0x00 - 0x11 (0..17) on/off state for each LED (0=off) */
	/* 0x12 - 0x23 (18..35) blink status (0=no blink) */
	/* 0x24 - 0xb3 (36-180) PWM for each LED */
	/*	comlicated formula */

	int i;

	memset(buffer,0,182);
	buffer[2]=0;					// start register
	for(i=0;i<18;i++) {
		buffer[1+i]=0xff;	/* turn all on */
		buffer[1+0x12+i]=0x00;	/* turn blink off */
	}

	int count=0;
	while(1) {

		for(i=0;i<144;i++) {
			if (i==count) buffer[1+0x24+i]=0x40;
			else buffer[1+0x24+i]=0x00;
		}

		result=write(i2c_fd,buffer,182);
		if ((result<0) || (result!=182)) {
			fprintf(stderr,"Error configuring\n");
		}

		count++;
		if (count>144) count=0;

		usleep(500000);

	}

	close(i2c_fd);

	return 0;
}

