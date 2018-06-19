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

static char *key_lookup[5][7]= {

	{
		"q",	// 0,0
		"w",	// 0,1
		"sym",	// 0,2
		"a",	// 0,3
		"alt",	// 0,4
		" ",	// 0,5
		"mic",	// 0,6
	},
	{
		"e",	// 1,0
		"s",	// 1,1
		"d",	// 1,2
		"p",	// 1,3
		"x",	// 1,4
		"z",	// 1,5
		"lshft",// 1,6
	},
	{
		"r",	// 2,0
		"g",	// 2,1
		"t",	// 2,2
		"rshft",// 2,3
		"v",	// 2,4
		"c",	// 2,5
		"f",	// 2,6
	},
	{
		"u",	// 3,0
		"h",	// 3,1
		"y",	// 3,2
		"enter",// 3,3
		"b",	// 3,4
		"n",	// 3,5
		"j",	// 3,6
	},
	{
		"o",	// 4,0
		"l",	// 4,1
		"i",	// 4,2
		"bs",	// 4,3
		"$",	// 4,4
		"m",	// 4,5
		"k",	// 4,6
	},
};

static void decode_keypress(int value) {

	int row,col;

	row=value&0x7;
	col=(value>>3)&0x7;

	printf("%x ",value&0x3f);
	printf("row=%d ",row);
	printf("col=%d ",col);
	printf("val= %s\n",key_lookup[col][row]);
	return;
}

int main(int argc, char **argv) {

	int result;
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

	buffer[0]=0x01; /* select debounce reg */

	buffer[1]=0x7f; /* debounce: Ports 7,6,5 enabled (col 0,1,2,3,4, enabled) */
	buffer[2]=0x00; /* autorepeat */
	buffer[3]=0x00; /* interrupt */
	buffer[4]=0x83;	/* config: enable device + enable sound */

	result=write(i2c_fd,buffer,5);

	printf("Result was %d, %s\n",result,strerror(errno));

//	buffer[0]=0x04; /* select config reg */
//	result=write(i2c_fd,buffer,1);

	buffer[0]=0x00; /* select FIFO reg */
	result=write(i2c_fd,buffer,1);

	while(1) {

		result=read(i2c_fd,buffer,1);
		if (buffer[0]!=0) {
			while(1) {
				decode_keypress(buffer[0]);
				if (buffer[0]&0x80) printf("FIFO overflow!\n");
				// 0x40 bit means more
				if (!(buffer[0]&0x40)) {
					break;
				}
				result=read(i2c_fd,buffer,1);
			}

		}
		usleep(100000);
	}

	close(i2c_fd);

	return result;
}

