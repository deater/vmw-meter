/* + An adafruit 8x16 led matrix backpack (0x72) 			  */

#include "display.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <termios.h>
#include <fcntl.h>

#if USE_LINUX_I2C==1
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include "i2c_lib.h"
#else
#include <bcm2835.h>
#endif

#include "font.h"

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


static int freq_max[16];
static int freq_matrix[16][8];



static int freq_8x16display(int display_type, int refresh_i2c) {

	unsigned char buffer[17];
	int i,x;


	buffer[0]=0;

	for(i=0;i<16;i++) buffer[i+1]=0x0;

	for(i=0;i<16;i++) {
		for(x=0;x<8;x++) {
			buffer[i+1]|=(freq_matrix[x+(8*(i%2))][i/2]<<x);
		}
	}

	if ((display_type&DISPLAY_I2C) && (!refresh_i2c)) {
		/* don't display if not time to refresh */
	}

	else {

		display_8x16_raw(display_type, buffer);
	}

	return 0;
}





#define UPDATE_DIVIDER	5

static int divider=0;

int display_8x16_freq(int display_type,
		int la, int lb, int lc,
		int ra, int rb, int rc) {

	int x,y;

	if (ra>=0) {
		if (ra>15) {
			ra=15;
		}
		freq_max[ra]=1;
		for(y=0;y<8;y++) freq_matrix[ra][y]=1;
	}
	if (la>=0) {
		if (la>15) {
			la=15;
		}
		freq_max[la]=1;
		for(y=0;y<8;y++) freq_matrix[la][y]=1;
	}


	if (rb>=0) {
		if (rb>15) {
			rb=15;
		}
		freq_max[rb]=1;
		for(y=0;y<8;y++) freq_matrix[rb][y]=1;
	}
	if (lb>=0) {
		if (lb>15) {
			lb=15;
		}
		freq_max[lb]=1;
		for(y=0;y<8;y++) freq_matrix[lb][y]=1;
	}


	if (rc>=0) {
		if (rc>15) {
			rc=15;
		}
		freq_max[rc]=1;
		for(y=0;y<8;y++) freq_matrix[rc][y]=1;
	}
	if (lc>=0) {
		if (lc>15) {
			lc=15;
		}
		freq_max[lc]=1;
		for(y=0;y<8;y++) freq_matrix[lc][y]=1;
	}


	for(x=0;x<16;x++) {
		freq_matrix[x][freq_max[x]]=7;
		if (divider==0)
		if (freq_max[x]<7) freq_max[x]++;
	}

	if (divider==0) {
		freq_8x16display(display_type,1);
	} else {
		freq_8x16display(display_type,0);
	}

	divider++;
	if (divider>UPDATE_DIVIDER) {
		divider=0;
		/* Clear background */

		for(y=0;y<8;y++) {
			for(x=0;x<16;x++) {
				freq_matrix[x][y]=0;
			}
		}
	}

	return 0;
}





static int put_number(int which, int x, int y) {

	int a,b;

	for(a=0;a<3;a++) {
		for(b=0;b<5;b++) {
			freq_matrix[a+x][b+y]=number_font[which][b][a];
		}
	}

	return 0;
}

int display_8x16_time(int display_type, int current_frame, int total_frames) {

	int x,y;
	int total_s,current_s,display_m,display_s,bar_length;

	/* Only update a few times a second? */
	/* Should do more for responsiveness? */
	if (current_frame%16!=0) {
		freq_8x16display(display_type,0);
		return 0;
	}

	/* clear display */
	for(x=0;x<16;x++) {
		for(y=0;y<8;y++) {
			freq_matrix[x][y]=0;
		}
	}

	total_s=total_frames/50;
	current_s=current_frame/50;

	if (total_s==0) bar_length=0;
	else bar_length=(current_s*14)/total_s;
//	printf("Bar length=%d/14\n",bar_length);

	for(x=0;x<bar_length;x++) {
		freq_matrix[x+1][6]=1;
	}

	display_m=current_s/60;
	display_s=current_s%60;
//	printf("%d:%d\n",display_m,display_s);

	/* Print tens char */
	/* Don't handle files >= 20 minutes well */
	if (display_m>9) {
		for(y=0;y<5;y++) freq_matrix[1][y]=1;
	}

	put_number(display_m,3,0);

	/* Draw Colon */
	freq_matrix[7][1]=1;
	freq_matrix[7][3]=1;

	put_number(display_s/10,9,0);
	put_number(display_s%10,13,0);

	freq_8x16display(display_type,1);

	return 0;
}

int display_8x16_title(int display_type) {

	int x,y;
	static int count=0,offset=0,direction=+1;
	static int toggles=0;

	/* clear display */
	for(x=0;x<16;x++) {
		for(y=0;y<8;y++) {
			freq_matrix[x][y]=0;
		}
	}

	for(y=0;y<3;y++) {
		for(x=0;x<16;x++) {
			freq_matrix[x][y]=!!(vmw[y]&(1<<(31-x)));
		}
	}

	for(y=0;y<3;y++) {
		for(x=0;x<16;x++) {
			freq_matrix[x][y+4]=!!(chiptune[y]&(1<<(31-x-offset)));
		}
	}

	freq_8x16display(display_type,1);

	/* Only scroll at 1/6 of update time */
	count++;
	if (count>6) {
		count=0;
		offset+=direction;

		/* If scroll off end, change direction */
		if (offset>15) {
			offset=15;
			direction=-direction;
		}

		/* if scoll back to beginning, change direction */
		if (offset<0) {
			offset=0;
			direction=-direction;
			toggles++;
//			printf("Toggle %d\n",toggles);
		}
	}

	/* If not in kiosk mode only display title briefly */
	if (toggles) {
		return 1;
	}

	return 0;
}


int display_8x16_scroll_text(int display_type, char *string, int new_string) {

	/* 5x7 font, really 6x7 if you include space */
	/* 6*16 = 96 */
	char ascii[]=	" !\"#$%&\'()*+,-./"
			"0123456789:;<=>?"
			"@ABCDEFGHIJKLMNO"
			"PQRSTUVWXYZ[\\]^_"
			"`abcdefghijklmno"
			"pqrstuvwxyz{|}~ ";
//	char buf[16]="ABCDEFGHIJKLMNOP";
	char matrix[96][8];

	int scroll=0,x,y,length;
	static int i;
	static int frames=0;

	length=strlen(ascii);

	frames++;
	if (frames<50) {
		return 0;
	}
	else {
		frames=0;
		i++;
		if (i>length) i=0;
	}

	printf("Trying to print %c\n",ascii[i]);

	for(y=0;y<8;y++) {
		for(x=0;x<5;x++) {
			matrix[x][y]=!!(fbs_font[(int)ascii[i]][y]&1<<(4-x));
		}
		matrix[x][y]=0;
	}

	for(y=0;y<8;y++) {
		for(x=0;x<16;x++) {
			freq_matrix[x][y]=matrix[x+scroll][y];
		}
	}

	freq_8x16display(display_type,1);

	return 0;
}
