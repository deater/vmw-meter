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


int emulate_8x16_display(unsigned char *display_state) {

	int i,j;

	clear_screen();

	int current_color=0;

	for(i=0;i<16;i++) {
		for(j=7;j>=0;j--) {
			if (display_state[i]&1<<j) {
				if (current_color==0) {
					printf("\x1b[42m");
					current_color=1;
				}
				printf(" ");
			}
			else {
				if (current_color==1) {
					printf("\x1b[40m");
					current_color=0;
				}
				printf(" ");
			}
		}
		current_color=0;
		printf("\x1b[40m.\n");
	}
	printf("\x1b[0m");

	return 0;
}

static int change_color(int current_color,int new_color) {

	if (new_color) new_color=2;

	if (current_color==new_color) {
	}
	else {
		printf("\x1b[3%dm",new_color);
	}

	return new_color;
}

int emulate_4x7seg_display(unsigned short *display_state) {

	int current_color=0;

	/* line 1 */
	printf("\x1b[30;1m");
	current_color=change_color(current_color,display_state[0]&1);
	printf(" __ ");
	current_color=change_color(current_color,display_state[1]&1);
	printf("  __  ");
	current_color=change_color(current_color,display_state[3]&1);
	printf("   __ ");
	current_color=change_color(current_color,display_state[4]&1);
	printf("  __  \n");

	/* line 2 */
	current_color=change_color(current_color,display_state[0]&0x20);
	printf("| ");
	current_color=change_color(current_color,display_state[0]&0x02);
	printf(" | ");
	current_color=change_color(current_color,display_state[1]&0x20);
	printf("| ");
	current_color=change_color(current_color,display_state[1]&0x02);
	printf(" | ");
	current_color=change_color(current_color,display_state[2]&0x02);
	printf(".");
	current_color=change_color(current_color,display_state[3]&0x20);
	printf(" |");
	current_color=change_color(current_color,display_state[3]&0x02);
	printf("  |");
	current_color=change_color(current_color,display_state[4]&0x20);
	printf(" |");
	current_color=change_color(current_color,display_state[4]&0x02);
	printf("  | \n");

	/* line 3 */
	current_color=change_color(current_color,display_state[0]&0x40);
	printf(" -- ");
	current_color=change_color(current_color,display_state[1]&0x40);
	printf("  --  ");
	current_color=change_color(current_color,display_state[3]&0x40);
	printf("   -- ");
	current_color=change_color(current_color,display_state[4]&0x40);
	printf("  --  \n");

	/* line 4 */
	current_color=change_color(current_color,display_state[0]&0x10);
	printf("| ");
	current_color=change_color(current_color,display_state[0]&0x04);
	printf(" |");
	current_color=change_color(current_color,display_state[1]&0x10);
	printf(" |");
	current_color=change_color(current_color,display_state[1]&0x04);
	printf("  |");
	current_color=change_color(current_color,display_state[2]&0x02);
	printf(" .");
	current_color=change_color(current_color,display_state[3]&0x10);
	printf(" |");
	current_color=change_color(current_color,display_state[3]&0x04);
	printf("  |");
	current_color=change_color(current_color,display_state[4]&0x10);
	printf(" |");
	current_color=change_color(current_color,display_state[4]&0x04);
	printf("  | \n");

	/* line 5 */
	current_color=change_color(current_color,display_state[0]&0x08);
	printf(" -- ");
	current_color=change_color(current_color,display_state[0]&0x80);
	printf(".");
	current_color=change_color(current_color,display_state[1]&0x08);
	printf(" -- ");
	current_color=change_color(current_color,display_state[1]&0x80);
	printf(".");
	current_color=change_color(current_color,display_state[3]&0x08);
	printf("   -- ");
	current_color=change_color(current_color,display_state[3]&0x80);
	printf(".");
	current_color=change_color(current_color,display_state[4]&0x08);
	printf(" -- ");
	current_color=change_color(current_color,display_state[4]&0x80);
	printf(".\n");

	/* reset colors */
	printf("\x1b[0m");

	return 0;
}

int init_saa1064_display(int i2c_fd, int i2c_addr) {

	unsigned char buffer[6]="\x00\x47\x00\x00\x00\x00";


	/* set dynamic mode, noblank */
	/* notest, 12mA current, blank all segments */

	if (ioctl(i2c_fd, I2C_SLAVE, i2c_addr) < 0) {
		fprintf(stderr,"Error setting i2c address %x\n",i2c_addr);
		return -1;
	}

	if (write(i2c_fd,buffer,6)!=6) {
		fprintf(stderr,"Error writing to SAA1064 %d\n",i2c_addr);
		return -1;
	}

	return 0;

}

static unsigned short reverse_bits16(unsigned short v) {

	unsigned int s = 16;
	unsigned int mask = ~0;
	while ((s >>= 1) > 0) {
		mask ^= (mask << s);
		v = ((v >> s) & mask) | ((v << s) & ~mask);
	}
	return v;
}

/* non-reentrant :( */

static unsigned short existing_state[8]= {
   0x0,0x0,0x0,0x0,
   0x0,0x0,0x0,0x0,
};


int update_saa1064_display(int i2c_fd, int i2c_addr, unsigned short *in_state) {

	int i;
	char buffer[6];
	unsigned short temp;
	unsigned short state[8];

	/* c passes arrays by reference so we have to do this         */
	/* otherwise changes to state[] are propogated back to caller */
	for(i=0;i<8;i++) state[i]=in_state[i];

	/* state[0]=digit0 */
	/* state[1]=digit1 */
	/* state[2]=digit2 */
	/* state[3]=digit3 */
	/* state[4]=digit4 */
	/* state[5]=digit5 */
	/* state[6]=left bargraph */
	/* state[7]=right bargraph */

	for(i=0;i<4;i++) {

		/* the bargraph was wired backward on actual hardware */
		/* fix it here to make software easier to write       */
		if (i==3) {

			temp=(reverse_bits16(state[6])<<6)|(state[6]&0x3f);
			state[6]=temp;

			temp=(reverse_bits16(state[7])<<6)|(state[7]&0x3f);
			state[7]=temp;
		}

		/* only update if there's been a change */
		if ( (existing_state[i*2]!=state[i*2]) ||
			(existing_state[(i*2)+1]!=state[(i*2)+1])) {

			existing_state[i*2]=state[i*2];
			existing_state[(i*2)+1]=state[(i*2)+1];

			/* write out to hardware */
			if (ioctl(i2c_fd, I2C_SLAVE, i2c_addr+i) < 0) {
				fprintf(stderr,"Error setting i2c address %x\n",i2c_addr);
				return -1;
			}

			buffer[0]=0x01;
			buffer[1]=(state[i*2]>>8)&0xff;
			buffer[2]=(state[(i*2)+1]>>8)&0xff;
			buffer[3]=(state[i*2])&0xff;
			buffer[4]=(state[(i*2)+1])&0xff;
			if (write(i2c_fd,buffer,5)!=5) {
				fprintf(stderr,"Error writing!\n");
				return -1;
			}
		}
	}
	return 0;
}



static void red_or_grey(int condition) {

	if (condition) {
		/* bright red */
		printf("\033[1;31m");
	}
	else {
		/* dark grey */
		printf("\033[1;30m");
	}
}

static void bargraph(int condition1, int condition2) {

	if (condition1 && condition2) {
		/* bright green  */
		printf("\033[1;32m");
		printf(";; ");
	}
	else if (condition1) {
		/* bright green  */
		printf("\033[1;32m");
		printf(",, ");
	}
	else if (condition2) {
		/* bright green  */
		printf("\033[1;32m");
		printf("'' ");
	}
	else {
		/* dark grey */
		printf("\033[1;30m");
		printf(";; ");
	}
}

#define SAA1064_METER_DIGITS 8

int update_saa1064_ascii(unsigned short *global_meter_state) {

	int i;

	unsigned short meter_state[SAA1064_METER_DIGITS];

	for(i=0;i<8;i++) {
		meter_state[i]=global_meter_state[i];
	}

	/* clear screen */
	printf("\033[2J\n");

	/* move to upper left */
	printf("\033[1;1H");

	//                @ @ @ @ @ @
	//     @ @   @ @   @ @   @ @   @ @   @ @
	// ;;  ___   ___   ___   ___   ___   ___  ;;
	// ;; |\|/| |\|/| |\|/| |\|/| |\|/| |\|/| ;;
	// ;;  - -   - -   - -   - -   - -   - -  ;;
	// ;; |/|\| |/|\| |/|\| |/|\| |/|\| |/|\| ;;
	// ;;  ---   ---   ---   ---   ---   ---  ;;
	//

	/* first line */
	printf("                ");

	for(i=0;i<6;i++) {
		if (meter_state[i]&SAA1064_SEGMENT_EX) {
			switch(i) {
			case 0: printf("\033[0;31m"); break; /* bright red */
			case 1: printf("\033[1;31m"); break; /* bright orange */
			case 2: printf("\033[1;33m"); break; /* bright yellow */
			case 3: printf("\033[1;32m"); break; /* bright green */
			case 4: printf("\033[1;34m"); break; /* bright blue */
			case 5: printf("\033[1;35m"); break; /* bright purple */
			}
		}
		else {
			/* dark grey */
			printf("\033[1;30m");
		}
		printf("@ ");
	}
	printf("\n");

	/* second line */
	printf("     ");
	red_or_grey(meter_state[6]&SAA1064_SEGMENT_L);
	printf("@ ");
	red_or_grey(meter_state[6]&SAA1064_SEGMENT_M);
	printf("@   ");
	red_or_grey(meter_state[6]&SAA1064_SEGMENT_N);
	printf("@ ");
	red_or_grey(meter_state[6]&SAA1064_SEGMENT_P);
	printf("@   ");
	red_or_grey(meter_state[6]&SAA1064_SEGMENT_DP);
	printf("@ ");
	red_or_grey(meter_state[6]&SAA1064_SEGMENT_EX);
	printf("@   ");

	red_or_grey(meter_state[7]&SAA1064_SEGMENT_EX);
	printf("@ ");
	red_or_grey(meter_state[7]&SAA1064_SEGMENT_DP);
	printf("@   ");
	red_or_grey(meter_state[7]&SAA1064_SEGMENT_P);
	printf("@ ");
	red_or_grey(meter_state[7]&SAA1064_SEGMENT_N);
	printf("@   ");
	red_or_grey(meter_state[7]&SAA1064_SEGMENT_M);
	printf("@ ");
	red_or_grey(meter_state[7]&SAA1064_SEGMENT_L);
	printf("@");
	printf("\n");

	/* third line */
	printf(" ");
	bargraph(meter_state[6]&SAA1064_SEGMENT_J,
		meter_state[6]&SAA1064_SEGMENT_K);
	for(i=0;i<6;i++) {
		red_or_grey(meter_state[i]&SAA1064_SEGMENT_A);
		printf(" ___  ");
	}
	bargraph(meter_state[7]&SAA1064_SEGMENT_J,
		meter_state[7]&SAA1064_SEGMENT_K);
	printf("\n");

	/* fourth line */
	printf(" ");
	bargraph(meter_state[6]&SAA1064_SEGMENT_G,meter_state[6]&SAA1064_SEGMENT_H);
	for(i=0;i<6;i++) {
		red_or_grey(meter_state[i]&SAA1064_SEGMENT_F);
		printf("|");
		red_or_grey(meter_state[i]&SAA1064_SEGMENT_P);
		printf("\\");
		red_or_grey(meter_state[i]&SAA1064_SEGMENT_G);
		printf("|");
		red_or_grey(meter_state[i]&SAA1064_SEGMENT_H);
		printf("/");
		red_or_grey(meter_state[i]&SAA1064_SEGMENT_B);
		printf("| ");
	}
	bargraph(meter_state[7]&SAA1064_SEGMENT_G,meter_state[7]&SAA1064_SEGMENT_H);
	printf("\n");

	/* fifth line */
	printf(" ");
	bargraph(meter_state[6]&SAA1064_SEGMENT_E,meter_state[6]&SAA1064_SEGMENT_F);
	for(i=0;i<6;i++) {
		red_or_grey(meter_state[i]&SAA1064_SEGMENT_N);
		printf(" - ");
		red_or_grey(meter_state[i]&SAA1064_SEGMENT_J);
		printf("-  ");
	}
	bargraph(meter_state[7]&SAA1064_SEGMENT_E,meter_state[7]&SAA1064_SEGMENT_F);
	printf("\n");

	/* sixth line */
	printf(" ");
	bargraph(meter_state[6]&SAA1064_SEGMENT_C,meter_state[6]&SAA1064_SEGMENT_D);
	for(i=0;i<6;i++) {
		red_or_grey(meter_state[i]&SAA1064_SEGMENT_E);
		printf("|");
		red_or_grey(meter_state[i]&SAA1064_SEGMENT_M);
		printf("/");
		red_or_grey(meter_state[i]&SAA1064_SEGMENT_L);
		printf("|");
		red_or_grey(meter_state[i]&SAA1064_SEGMENT_K);
		printf("\\");
		red_or_grey(meter_state[i]&SAA1064_SEGMENT_C);
		printf("| ");
	}
	bargraph(meter_state[7]&SAA1064_SEGMENT_C,meter_state[7]&SAA1064_SEGMENT_D);
	printf("\n");

	/* seventh line */
	printf(" ");
	bargraph(meter_state[6]&SAA1064_SEGMENT_A,meter_state[6]&SAA1064_SEGMENT_B);
	for(i=0;i<6;i++) {
		red_or_grey(meter_state[i]&SAA1064_SEGMENT_D);
		printf(" --- ");
		red_or_grey(meter_state[i]&SAA1064_SEGMENT_DP);
		printf(".");
	}
	bargraph(meter_state[7]&SAA1064_SEGMENT_A,meter_state[7]&SAA1064_SEGMENT_B);
	printf("\n");

	/* restore default colors */
	printf("\033[0;39;49m\n");

	return 0;
}

