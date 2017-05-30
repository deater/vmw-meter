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

#include "14seg_font.h"

short adafruit_lookup[256];

/* translate 14seg font to adafruit pinout */
int setup_14seg_font(void) {

	int i;

	for(i=0;i<256;i++) {

		adafruit_lookup[i]=0;

		if (ascii_lookup[i]&(1<<15)) adafruit_lookup[i]|=(1<< 8); // A
		if (ascii_lookup[i]&(1<<14)) adafruit_lookup[i]|=(1<< 9); // B
		if (ascii_lookup[i]&(1<<13)) adafruit_lookup[i]|=(1<<10); // C
		if (ascii_lookup[i]&(1<<12)) adafruit_lookup[i]|=(1<<11); // D
		if (ascii_lookup[i]&(1<<11)) adafruit_lookup[i]|=(1<<12); // E
		if (ascii_lookup[i]&(1<<10)) adafruit_lookup[i]|=(1<<13); // F
		if (ascii_lookup[i]&(1<< 9)) adafruit_lookup[i]|=(1<< 1); // G
		if (ascii_lookup[i]&(1<< 8)) adafruit_lookup[i]|=(1<< 2); // H
		if (ascii_lookup[i]&(1<< 7)) adafruit_lookup[i]|=(1<<15); // J
		if (ascii_lookup[i]&(1<< 6)) adafruit_lookup[i]|=(1<< 5); // K
		if (ascii_lookup[i]&(1<< 5)) adafruit_lookup[i]|=(1<< 4); // L
		if (ascii_lookup[i]&(1<< 4)) adafruit_lookup[i]|=(1<< 3); // M
		if (ascii_lookup[i]&(1<< 3)) adafruit_lookup[i]|=(1<<14); // N
		if (ascii_lookup[i]&(1<< 2)) adafruit_lookup[i]|=(1<< 0); // P
		if (ascii_lookup[i]&(1<< 1)) adafruit_lookup[i]|=(1<< 6); // .
	}

	return 0;
}




int display_14seg_string(int display_type,char *led_string) {

	static char old_buffer1[17],old_buffer2[17],old_buffer3[17];
	char buffer1[17],buffer2[17],buffer3[17];
	int i,ch;

	buffer1[0]=0;
	buffer2[0]=0;
	buffer3[0]=0;

	if (display_type&DISPLAY_TEXT) {
		printf("%s\n",led_string);
		return 0;
	}

	/* else i2c */

	for(i=0;i<4;i++) {
		ch=led_string[i];
		buffer1[(i*2)+1]=adafruit_lookup[ch]>>8;
		buffer1[(i*2)+2]=adafruit_lookup[ch]&0xff;
	}

	for(i=0;i<4;i++) {
		ch=led_string[i+4];
		buffer2[(i*2)+1]=adafruit_lookup[ch]>>8;
		buffer2[(i*2)+2]=adafruit_lookup[ch]&0xff;
	}

	for(i=0;i<4;i++) {
		ch=led_string[i+8];
		buffer3[(i*2)+1]=adafruit_lookup[ch]>>8;
		buffer3[(i*2)+2]=adafruit_lookup[ch]&0xff;
	}

	if (memcmp(buffer1,old_buffer1,17)!=0) {

#if USE_LINUX_I2C==1
		if (ioctl(i2c_fd, I2C_SLAVE, HT16K33_ADDRESS5) < 0) {
			fprintf(stderr,"String1 error setting i2c address %x\n",
				HT16K33_ADDRESS5);
			return -1;
		}

		if ( (write(i2c_fd, buffer1, 17)) !=17) {
			fprintf(stderr,"Error writing display %s!\n",
				strerror(errno));
			return -1;
		}
#else
		bcm2835_i2c_setSlaveAddress(0x75);
		bcm2835_i2c_write(buffer1,17);
#endif
		memcpy(old_buffer1,buffer1,17);
	}

	if (memcmp(buffer2,old_buffer2,17)!=0) {
#if USE_LINUX_I2C==1
		if (ioctl(i2c_fd, I2C_SLAVE, HT16K33_ADDRESS3) < 0) {
			fprintf(stderr,"String2 error setting i2c address %x\n",
				HT16K33_ADDRESS3);
			return -1;
		}

		if ( (write(i2c_fd, buffer2, 17)) !=17) {
			fprintf(stderr,"Error writing display %s!\n",
				strerror(errno));
			return -1;
		}
#else
			bcm2835_i2c_setSlaveAddress(0x73);
			bcm2835_i2c_write(buffer2,17);
#endif
		memcpy(old_buffer2,buffer2,17);
	}


	if (memcmp(buffer3,old_buffer3,17)!=0) {
#if USE_LINUX_I2C==1
		if (ioctl(i2c_fd, I2C_SLAVE, HT16K33_ADDRESS7) < 0) {
			fprintf(stderr,"String3 error setting i2c address %x\n",
				HT16K33_ADDRESS7);
			return -1;
		}

		if ( (write(i2c_fd, buffer3, 17)) !=17) {
			fprintf(stderr,"Error writing display %s!\n",
				strerror(errno));
			return -1;
		}
#else
		bcm2835_i2c_setSlaveAddress(0x77);
		bcm2835_i2c_write(buffer3,17);

#endif
		memcpy(old_buffer3,buffer3,17);
	}

	return 0;

}

int close_14seg(int type) {

	char buffer[13];

	strcpy(buffer,"            ");
	display_14seg_string(type,buffer);

	return 0;

}
