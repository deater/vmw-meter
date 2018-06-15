/* Tries to work keypad on MAX7349 */
/* Assumes hooked up to Blackberry Q10 keypad */
/* Useful info here: https://hackaday.io/project/27520-pimp-personal-information-manager-pager/log/69681-blackberry-q10-keyboard-destructive-disassembly */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/ioctl.h>

#include "i2c_lib.h"

#include <linux/i2c-dev.h>

int main(int argc, char **argv) {

	int result,i;
	int i2c_fd;
	unsigned char buffer[256];

	int keyscan_addr=0x38;
	int sounder_addr=0x39; // assume AD0 = 0

	i2c_fd=init_i2c("/dev/i2c-1");
	if (i2c_fd < 0) {
		fprintf(stderr,"Error opening device! %s\n",
			strerror(errno));
		return -1;
	}


	if (ioctl(i2c_fd, I2C_SLAVE, keyscan_addr) < 0) {
		fprintf(stderr,"error setting i2c address %x\n",sounder_addr);
		return -1;
	}

	/* Configure the device */

//	buffer[0]=0x00; /* fifo: power up default */
//	buffer[1]=0xff; /* debounce (power up default) */
//	buffer[2]=0x00; /* autorepeat (power up default) */
//	buffer[3]=0x00; /* interrupt: disable */
//	buffer[4]=0x83;	/* config: enable device + enable sound */

	buffer[0]=0x04; /* select config reg */
	buffer[1]=0x83;	/* config: enable device + enable sound */

	result=write(i2c_fd,buffer,2);

	printf("Result was %d, %s\n",result,strerror(errno));

//	buffer[0]=0x04; /* select config reg */
//	result=write(i2c_fd,buffer,1);

	buffer[0]=0x00; /* select FIFO reg */
	result=write(i2c_fd,buffer,1);

	while(1) {

		result=read(i2c_fd,buffer,1);
		printf("%x\n",buffer[0]);
		sleep(1);
	}

	close(i2c_fd);

	return result;
}

