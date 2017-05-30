/* This code assumes you have a VMW-HARDWARE-CHIPTUNES-DISPLAY-MK1        */
/* This includes five ht16k33 based displays hooked to the first i2c bus: */
/* + A ht16k33 breakout with six 10-segment GYR bargraphs (0x70)	  */
/*   also 8 buttons are hooked up to the ht16k33 breakout.		  */
/* + An adafruit 8x16 led matrix backpack (0x72) 			  */
/* + Three adafruit 14seg led backpacks (..,..,..)			  */

/* Be sure to modprobe i2c-dev */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <termios.h>
#include <fcntl.h>

#include "display.h"

#if USE_LINUX_I2C==1
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include "i2c_lib.h"
#else
#include <bcm2835.h>
#endif




static int bargraph_i2c(int left_a, int left_b, int left_c,
			int right_a, int right_b, int right_c) {

	int i;

	char buffer[17];

	buffer[0]=0;

	for(i=0;i<16;i++) buffer[i+1]=0x0;

	/* a */
	if (left_a>0) {
		left_a--;
		/* left */
		buffer[1]|=(2<<left_a)-1;
		if (left_a>7) buffer[2]|=(2<<(left_a-8))-1;
	}
	if (right_a>0) {
		right_a--;
		/* right */
		buffer[7]|=(2<<right_a)-1;
		if (right_a>7) buffer[8]|=(2<<(right_a-8))-1;
	}

	/* b */
	if (left_b>0) {
		left_b--;
		/* left */
		buffer[3]|=(2<<left_b)-1;
		if (left_b>7) buffer[4]|=(2<<(left_b-8))-1;
	}
	if (right_b>0) {
		right_b--;
		/* right */
		buffer[9]|=(2<<right_b)-1;
		if (right_b>7) buffer[10]|=(2<<(right_b-8))-1;
	}

	/* c */
	if (left_c>0) {
		left_c--;
		/* left */
		buffer[5]|=(2<<left_c)-1;
		if (left_c>7) buffer[6]|=(2<<(left_c-8))-1;
	}
	if (right_c>0) {
		right_c--;
		/* right */
		buffer[11]|=(2<<right_c)-1;
		if (right_c>7) buffer[12]|=(2<<(right_c-8))-1;
	}

#if USE_LINUX_I2C==1
	if (ioctl(i2c_fd, I2C_SLAVE, HT16K33_ADDRESS0) < 0) {
		fprintf(stderr,"Bargraph error setting i2c address %x\n",
			HT16K33_ADDRESS0);
		return -1;
	}

	if ( (write(i2c_fd, buffer, 17)) !=17) {
		fprintf(stderr,"Error writing display %s!\n",
			strerror(errno));
		return -1;
        }
#else
	bcm2835_i2c_setSlaveAddress(0x70);
	bcm2835_i2c_write(buffer,17);
#endif

	return 0;
}

static int bargraph_text(int left_a, int left_b, int left_c,
			int right_a,int right_b,int right_c) {

	int i,j,value;

	for(j=0;j<3;j++) {
		if (j==0) value=left_a;
		if (j==1) value=left_b;
		if (j==2) value=left_c;

		if (value>10) value=10;

		printf("[");
		for(i=0;i<value;i++) printf("*");
		for(i=value;i<10;i++) printf(" ");
		printf("]\n");
	}
	return 0;
}


int bargraph(int type,
	int left_a, int left_b, int left_c,
	int right_a, int right_b, int right_c) {

	if (type&DISPLAY_I2C) {
		bargraph_i2c(left_a,left_b,left_c,
				right_a,right_b,right_c);
	}

	if (type&DISPLAY_TEXT) {
		bargraph_text(left_a,left_b,left_c,
				right_a,right_b,right_c);
	}

	return 0;
}


int close_bargraph(int type) {

	if (type&DISPLAY_I2C) {
		bargraph_i2c(0,0,0,0,0,0);
	}

	return 0;
}
