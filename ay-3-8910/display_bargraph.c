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

#include "ymlib/stats.h"

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
	buffer[1]=left_a&0xff;
	buffer[2]=(left_a>>8)&0x3;
	buffer[7]=right_a&0xff;
	buffer[8]=(right_a>>8)&0x3;

	/* b */
	buffer[3]=left_b&0xff;
	buffer[4]=(left_b>>8)&0x3;
	buffer[9]=right_b&0xff;
	buffer[10]=(right_b>>8)&0x3;

	/* c */
	buffer[5]=left_c&0xff;
	buffer[6]=(left_c>>8)&0x3;
	buffer[11]=right_c&0xff;
	buffer[12]=(right_c>>8)&0x3;

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
		for(i=0;i<10;i++) {
			if (value&(1<<i)) printf("*");
			else printf(" ");
		}
		printf("]\n");
	}
	return 0;
}


int bargraph_filled(int type, struct display_stats *ds) {

	int i;

	for(i=0;i<3;i++) {
		/* scale to fit 10-segment display */
		ds->left_amplitude[i]=(ds->left_amplitude[i]*11)/16;

		if (ds->left_amplitude[i]>0) {
			ds->left_amplitude[i]--;
			ds->left_amplitude[i]=(2<<ds->left_amplitude[i])-1;
		}

		ds->right_amplitude[i]=(ds->right_amplitude[i]*11)/16;
		if (ds->right_amplitude[i]>0) {
			ds->right_amplitude[i]--;
			ds->right_amplitude[i]=(2<<ds->right_amplitude[i])-1;
		}
	}

	if (type&DISPLAY_I2C) {
		bargraph_i2c(	ds->left_amplitude[0],
				ds->left_amplitude[1],
				ds->left_amplitude[2],
				ds->right_amplitude[0],
				ds->right_amplitude[1],
				ds->right_amplitude[2]);
	}

	if (type&DISPLAY_TEXT) {
		bargraph_text(	ds->left_amplitude[0],
				ds->left_amplitude[1],
				ds->left_amplitude[2],
				ds->right_amplitude[0],
				ds->right_amplitude[1],
				ds->right_amplitude[2]);

	}

	return 0;
}

int bargraph_raw(int type,
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
