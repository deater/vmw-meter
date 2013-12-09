#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <termios.h>

#include <linux/i2c-dev.h>

#include "i2c_lib.h"

#define HT16K33_REGISTER_ADDRESS_POINTER	0x00
#define HT16K33_REGISTER_SYSTEM_SETUP		0x20
#define HT16K33_REGISTER_KEY_DATA_POINTER	0x40
#define HT16K33_REGISTER_INT_ADDRESS_POINTER	0x60
#define HT16K33_REGISTER_DISPLAY_SETUP		0x80
#define HT16K33_REGISTER_ROW_INT_SET		0xA0
#define HT16K33_REGISTER_TEST_MODE		0xD0
#define HT16K33_REGISTER_DIMMING		0xE0

/* Blink rate */
#define HT16K33_BLINKRATE_OFF			0x00
#define HT16K33_BLINKRATE_2HZ			0x01
#define HT16K33_BLINKRATE_1HZ			0x02
#define HT16K33_BLINKRATE_HALFHZ		0x03

int shutdown_display(int i2c_fd) {

	if (i2c_fd>=0) {
		return close(i2c_fd);
	}

	return -1;
}



void reset_display(unsigned short *display_state) {

	int i;

	for(i=0;i<DISPLAY_LINES;i++) {
		display_state[i]=0;
	}
}

/* Takes in 8 shorts */
/*  76543210 <-x     */
/*          0        */
/*          1        */
/*        ...        */
/*          7        */
/* And maps to actual output */
/*  70123456         */
/*          0        */
/*          1        */
/*         ...       */
/*          7        */

int update_8x8_display_rotated(int i2c_fd, int i2c_addr,
				unsigned char *display_state,
				int degrees) {

	unsigned char buffer[17];

	unsigned char rotated_display[8];

	int i,x,y;

	if (ioctl(i2c_fd, I2C_SLAVE, i2c_addr) < 0) {
		fprintf(stderr,"Error setting i2c address %x\n",
			i2c_addr);
		return -1;
 	}

	/* TODO: only update if there's been a change ?*/


	/* write out to hardware */
	buffer[0]=0x00;

	/* clear rotated_display */
	for(i=0;i<8;i++) rotated_display[i]=0;

	switch(degrees) {
		case 0:	/* No Rotate */
			for(i=0;i<8;i++) rotated_display[i]=display_state[i];
			break;
		case 90: /* Rotate 90 degrees clockwise */
			for(y=0;y<8;y++) {
				for(x=0;x<8;x++) {
					rotated_display[y]|=
					(!! (display_state[x]&(1<<(7-y))))<<x;
				}
			}
			break;
		case 180: /* Rotate 180 degrees clockwise */
			for(y=0;y<8;y++) {
				for(x=0;x<8;x++) {
					rotated_display[y]|=
					(!! (display_state[7-y]&(1<<(x))))<<(7-x);
				}
			}
			break;
		case 270: /* Rotate 270 degrees clockwise */
			for(y=0;y<8;y++) {
				for(x=0;x<8;x++) {
					rotated_display[y]|=
					(!! (display_state[7-x]&(1<<(y))))<<x;
				}
			}

			break;

		default:	fprintf(stderr,"Invalid rotation: %d\n",degrees);
				return -1;
	}

	/* ugh bug in backpack?  bit 0 is actually LED 1, bit 128 LED 0 */
	/* Verify that somehow the python code is outputting like this */
	/* Fix bits mirrored error */

	for(i=0;i<8;i++) {
		unsigned char temp;

		/* reverse bits */
		temp = (rotated_display[i]&0x55)<<1 |
			(rotated_display[i]&0xaa)>>1;
		temp=(temp&0x33) << 2 |
			(temp&0xcc) >> 2;
		temp = (temp&0x0f) << 4 |
			(temp&0xF0) >> 4;

		/* rotate right by 1 */
		buffer[(i*2)+1]=(temp>>1) | ((temp&0x1)<<7);
	}


	if ( (write(i2c_fd, buffer, 17)) !=17) {
		fprintf(stderr,"Erorr writing display!\n");
		return -1;
	}

	return 0;
}


int update_display(int i2c_fd, int i2c_addr, unsigned short *display_state) {

	unsigned char buffer[17];

	int i;

	if (ioctl(i2c_fd, I2C_SLAVE, i2c_addr) < 0) {
		fprintf(stderr,"Error setting i2c address %x\n",
			i2c_addr);
		return -1;
	}

	buffer[0]=0x00;

	for(i=0;i<8;i++) {
		buffer[1+(i*2)]=display_state[i]&0xff;
		buffer[2+(i*2)]=(display_state[i]>>8)&0xff;
	}

	if ( (write(i2c_fd, buffer, 17)) !=17) {
		fprintf(stderr,"Erorr writing display!\n");
      		return -1;
	}

	return 0;
}


/* Set brightness from 0 - 15 */
int set_brightness(int i2c_fd, int i2c_addr, int value) {

	unsigned char buffer[17];

	if ((value<0) || (value>15)) {
		fprintf(stderr,"Brightness value of %d out of range (0-15)\n",
			value);
		return -1;
	}

	if (ioctl(i2c_fd, I2C_SLAVE, i2c_addr) < 0) {
		fprintf(stderr,"Error setting i2c address %x\n",
			i2c_addr);
		return -1;
	}

	/* Set Brightness */
	buffer[0]= HT16K33_REGISTER_DIMMING | value;
	if ( (write(i2c_fd, buffer, 1)) !=1) {
		fprintf(stderr,"Error setting brightness!\n");
		return -1;
	}

	return 0;
}

/* Read keypad */
long long read_keypad(int i2c_fd, int i2c_addr) {

	unsigned char keypad_buffer[6];
	unsigned char buffer[17];
	long long keypress;

	if (ioctl(i2c_fd, I2C_SLAVE, i2c_addr) < 0) {
		fprintf(stderr,"Error setting i2c address %x\n",
			i2c_addr);
		return -1;
	}

	buffer[0]= HT16K33_REGISTER_KEY_DATA_POINTER;
	if ( (write(i2c_fd, buffer, 1)) !=1) {
		fprintf(stderr,"Error setting data_pointer!\n");
      		return -1;
	}

	read(i2c_fd,keypad_buffer,6);

	//for(i=0;i<6;i++) printf("%x ",keypad_buffer[i]);
	//printf("\n");

	keypress = (long long)keypad_buffer[0] |
		((long long)keypad_buffer[1]<<8) |
		((long long)keypad_buffer[2]<<16) |
		((long long)keypad_buffer[3]<<24) |
		((long long)keypad_buffer[4]<<32) |
		((long long)keypad_buffer[5]<<40);

	return keypress;
}


/* should make the device settable */
int init_display(int i2c_fd, int i2c_addr, int brightness) {

	unsigned char buffer[17];

	if (ioctl(i2c_fd, I2C_SLAVE, i2c_addr) < 0) {
		fprintf(stderr,"Error setting i2c address %x\n",
			i2c_addr);
		return -1;
	}

	/* Turn the oscillator on */
	buffer[0]= HT16K33_REGISTER_SYSTEM_SETUP | 0x01;
	if ( (write(i2c_fd, buffer, 1)) !=1) {
		fprintf(stderr,"Error starting display!\n");
		return -1;
	}

	/* Turn Display On, No Blink */
	buffer[0]= HT16K33_REGISTER_DISPLAY_SETUP | HT16K33_BLINKRATE_OFF | 0x1;
	if ( (write(i2c_fd, buffer, 1)) !=1) {
		fprintf(stderr,"Error starting display!\n");
		return -1;
	}

	set_brightness(i2c_fd, i2c_addr, brightness);

	return 0;
}


int init_nunchuck(int i2c_fd) {

	unsigned char buffer[17];

	if (ioctl(i2c_fd, I2C_SLAVE, WII_NUNCHUCK_ADDRESS) < 0) {
		fprintf(stderr,"Error setting i2c address %x\n",
			WII_NUNCHUCK_ADDRESS);
		return -1;
        }

	/* Start the nunchuck */
	/* Note: official Nintendo one apparently works by sending 0x40/0x00 */
	/* but third party ones you sed 0xf0/0x55/0xfb/0x00 ???		     */
	/* info from: http://wiibrew.org/wiki/Wiimote/Extension_Controllers/Nuncuck */

        buffer[0]=0xf0;
        buffer[1]=0x55;

	if ( (write(i2c_fd, buffer, 2)) !=2) {
		fprintf(stderr,"Error starting nunchuck! %s\n",
			strerror(errno));
		return -1;
	}

	buffer[0]=0xfb;
	buffer[1]=0x00;

	if ( (write(i2c_fd, buffer, 2)) !=2) {
		fprintf(stderr,"Error starting nunchuck! %s\n",
			strerror(errno));
		return -1;
	}

	return 0;

}


int read_nunchuck(int i2c_fd, struct nunchuck_data *results) {

	char buffer[6];
	int result;

	if (ioctl(i2c_fd, I2C_SLAVE, WII_NUNCHUCK_ADDRESS) < 0) {
		fprintf(stderr,"Error setting i2c address %x\n",
			WII_NUNCHUCK_ADDRESS);
		return -1;
        }

	buffer[0]=0x00;
	if ( (write(i2c_fd, buffer, 1)) !=1) {
		fprintf(stderr,"Error enabling read!\n");
		return -1;
	}

	/* needed? */
	// usleep(100000);

	result=read(i2c_fd,buffer,6);

	if (result!=6) {
		printf("Error reading %d\n",result);
	}

	results->joy_x=buffer[0];
	results->joy_y=buffer[1];
	results->acc_x=(buffer[2]<<2) | ((buffer[5]>>2)&0x3);
        results->acc_y=(buffer[3]<<2) | ((buffer[5]>>4)&0x3);
        results->acc_z=(buffer[4]<<2) | ((buffer[5]>>6)&0x3);
	results->z_pressed=!(buffer[5]&1);
	results->c_pressed=!((buffer[5]&2)>>1);

	return 0;
}


int init_i2c(char *device) {

	int i2c_fd=-1;

	i2c_fd = open(device, O_RDWR);
	if (i2c_fd < 0) {
		fprintf(stderr,"Error opening i2c dev file %s\n",device);
		return -1;
	}

	return i2c_fd;
}



static struct termios old_term;

int init_keyboard(void) {

	struct termios new_term;

	/* save current terminal settings for fd 0 (stdin) */
	tcgetattr (0, &old_term);

	/* get the current terminal settings to modify */
	tcgetattr (0, &new_term);

	/* Unset ICANON "canonical mode" bit */
	/* Canonical mode means one-line at a time wait until enter */
	new_term.c_lflag &= ~ICANON;

	/* Set VMIN (minimum number of chars to input before returning  */
	/* from read) to 1						*/
	new_term.c_cc[VMIN] = 1;

	/* Don't echo characters to the display */
	new_term.c_lflag &= ~ECHO;

	/* Set stdin to the new settings */
	tcsetattr (0, TCSANOW, &new_term);

	/* Set stdin to Non-Blocking, meaning don't wait for keypresses */
	/* Just return 0 if nothing being pressed.			*/
	fcntl (0, F_SETFL, fcntl (0, F_GETFL) | O_NONBLOCK);


	return 0;
}


int read_keyboard(void) {

	int ch=0,result;

	/* read from stdin */
	result=read(0,&ch,1);

	if (result!=-1) {
		/* See if was special "escape" character */
		if (ch==27) {
			result=read(0,&ch,1);
			if (result!=-1) {
				if (ch=='[') {
					result=read(0,&ch,1);
					if (result!=-1) {
						switch(ch) {
						case 'A': return KEYBOARD_UP;
						case 'B': return KEYBOARD_DOWN;
						case 'C': return KEYBOARD_RIGHT;
						case 'D': return KEYBOARD_LEFT;
						default: return KEYBOARD_UNKNOWN;
						}
					}
				}
			}
		}

		else {
			return ch;
		}
	}
	return -1;

}

int reset_keyboard(void) {
	/* Restore original settings */
	tcsetattr (0, TCSANOW, &old_term);

	return 0;
}

void clear_screen(void) {

	/* clear screen */
	printf("\033[2J\n");

	/* move to upper left */
	printf("\033[1;1H");

}

int emulate_8x8_display(unsigned char *display_state) {

	int i,j;

	clear_screen();

	for(i=0;i<8;i++) {
		for(j=0;j<8;j++) {
			if (display_state[i]&1<<j) printf("*");
			else printf(" ");
		}
		printf("\n");
	}
	return 0;
}
