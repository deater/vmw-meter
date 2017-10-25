/* + An adafruit 8x16 led matrix backpack (0x72) */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <termios.h>
#include <fcntl.h>

#include "stats.h"
#include "display.h"

#if USE_LINUX_I2C==1
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include "i2c_lib.h"
#else
#include <bcm2835.h>
#endif

/* Raw format for adafruit 8x16 display */
/* buffer[0] = 0   (says to start at address 0) */
/* buffer[1] = upper left, low bit first, 0 - 8 */
/* buffer[2] = upper right, low bit first, 0 - 8 */
/* .... */
/* buffer[15] = lower left  */
/* buffer[16] = lower right */

/* Code to put raw data on the 8x16 display */
int display_8x16_raw(int display_type, unsigned char *buffer) {

	int x,y;


	if (display_type&DISPLAY_I2C) {

#if USE_LINUX_I2C

		if (ioctl(i2c_fd, I2C_SLAVE, HT16K33_ADDRESS2) < 0) {
			fprintf(stderr,"8x16 Error setting i2c address %x\n",
					HT16K33_ADDRESS2);
			return -1;
		}

		if ( (write(i2c_fd, buffer, 17)) !=17) {
			fprintf(stderr,"Error writing display %s!\n",
					strerror(errno));
			return -1;
        	}
#else
		bcm2835_i2c_setSlaveAddress(0x72);
		bcm2835_i2c_write(buffer,17);
#endif

	}

	if (display_type&DISPLAY_TEXT) {
		for(y=0;y<8;y++) {
			for(x=0;x<8;x++) {
				if (buffer[1+(y*2)]&(1<<x)) printf("*");
				else printf(" ");
			}
			for(x=0;x<8;x++) {
				if (buffer[2+(y*2)]&(1<<x)) printf("*");
				else printf(" ");
			}
			printf("\n");
		}
	}
	return 0;
}


/* Raw format for adafruit 8x16 display */
/* buffer[0] = 0   (says to start at address 0) */
/* buffer[1] = upper left, low bit first, 0 - 8 */
/* buffer[2] = upper right, low bit first, 0 - 8 */
/* .... */
/* buffer[15] = lower left  */
/* buffer[16] = lower right */

int display_8x16_vertical(int display_type, unsigned char *in_buffer) {

	unsigned char buffer[17];
	int i;
	int mask;

	memset(buffer,0,17);

	for(i=0;i<8;i++) {
		mask=(1<<(7-i));

		buffer[(i*2)+1]=(!!(in_buffer[0]&(mask)) << 0) |
				(!!(in_buffer[1]&(mask)) << 1) |
				(!!(in_buffer[2]&(mask)) << 2) |
				(!!(in_buffer[3]&(mask)) << 3) |
				(!!(in_buffer[4]&(mask)) << 4) |
				(!!(in_buffer[5]&(mask)) << 5) |
				(!!(in_buffer[6]&(mask)) << 6) |
				(!!(in_buffer[7]&(mask)) << 7);

		buffer[(i*2)+2]=(!!(in_buffer[ 8]&(mask)) << 0) |
				(!!(in_buffer[ 9]&(mask)) << 1) |
				(!!(in_buffer[10]&(mask)) << 2) |
				(!!(in_buffer[11]&(mask)) << 3) |
				(!!(in_buffer[12]&(mask)) << 4) |
				(!!(in_buffer[13]&(mask)) << 5) |
				(!!(in_buffer[14]&(mask)) << 6) |
				(!!(in_buffer[15]&(mask)) << 7);

	}

	display_8x16_raw(display_type, buffer);

	return 0;
}


int display_8x16_vertical_inverted(int display_type, unsigned char *in_buffer) {

	unsigned char buffer[17];
	int i;
	int mask;

	memset(buffer,0,17);

	for(i=0;i<8;i++) {
		mask=(1<<(7-i));

		buffer[(i*2)+1]=(!!(in_buffer[7]&(mask)) << 0) |
				(!!(in_buffer[6]&(mask)) << 1) |
				(!!(in_buffer[5]&(mask)) << 2) |
				(!!(in_buffer[4]&(mask)) << 3) |
				(!!(in_buffer[3]&(mask)) << 4) |
				(!!(in_buffer[2]&(mask)) << 5) |
				(!!(in_buffer[1]&(mask)) << 6) |
				(!!(in_buffer[0]&(mask)) << 7);

		buffer[(i*2)+2]=(!!(in_buffer[15]&(mask)) << 0) |
				(!!(in_buffer[14]&(mask)) << 1) |
				(!!(in_buffer[13]&(mask)) << 2) |
				(!!(in_buffer[12]&(mask)) << 3) |
				(!!(in_buffer[11]&(mask)) << 4) |
				(!!(in_buffer[10]&(mask)) << 5) |
				(!!(in_buffer[ 9]&(mask)) << 6) |
				(!!(in_buffer[ 8]&(mask)) << 7);

	}

	display_8x16_raw(display_type, buffer);

	return 0;
}

/* Clear the display to black */
int close_8x16_display(int display_type) {

	int i;

	unsigned char buffer[17];

	buffer[0]=0;
	for(i=0;i<16;i++) buffer[i+1]=0x0;

	display_8x16_raw(display_type, buffer);

	return 0;
}


void display_8x16_vertical_putpixel(unsigned char *buffer,int x,int y) {

	int which,offset;

	if ((x<0) || (x>7)) return;
	if ((y<0) || (y>15)) return;

	which=(x*2)+(y<8);
	offset=1<<(7-(y&7));

	buffer[which]|=offset;
}

int display_8x16_vertical_getpixel(unsigned char *buffer,int x,int y) {

	int which,offset;

	if ((x<0) || (x>7)) return 0;
	if ((y<0) || (y>15)) return 0;

	which=(x*2)+(y<8);
	offset=1<<(7-(y&7));

	return !!(buffer[which]&offset);
}

static int reverse_bits(int b) {

	int out;

	out = ((b * 0x0802LU & 0x22110LU) |
		(b * 0x8020LU & 0x88440LU)) *
		0x10101LU >> 16;
	return out&0xff;
}

int display_8x16_horizontal(int display_type, unsigned char *in_buffer) {

	unsigned char buffer[17];
	int i;

	memset(buffer,0,17);

	for(i=0;i<8;i++) {
		buffer[i*2+1]=reverse_bits((in_buffer[i]>>8));
		buffer[i*2+2]=reverse_bits(in_buffer[i]&0xff);
	}

	display_8x16_raw(display_type, buffer);

	return 0;
}


/* Display some LED art */
int display_8x16_led_art(int display_type,
		short led_art[10][8],
		int which) {

	int i;
	unsigned char buffer[17];

	buffer[0]=0;

	/* clear buffer */
	for(i=0;i<16;i++) buffer[i+1]=0x0;

	if (which==1024) {
		/* special case, clear screen */
	} else {

		for(i=0;i<8;i++) {
			buffer[i*2+1]=reverse_bits((led_art[which][i]>>8));
			buffer[i*2+2]=reverse_bits(led_art[which][i]&0xff);
		}
	}

	display_8x16_raw(display_type, buffer);

	return 0;
}

