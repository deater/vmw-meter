/* assumes two ht16k33 displays hooked up on the first i2c bus	*/
/* first one is custom with 3 (eventually 6? 10 seg bargraphs	*/
/* second one is an adafruit 8x16 led matrix backpack		*/

/* The bargraphs are on i2c addr 0x70, the matrix on 0x72	*/

/* Be sure to modprobe i2c-dev */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include <sys/ioctl.h>
#include <linux/i2c-dev.h>


#include "i2c_lib.h"

#include "display.h"

#include "font.h"

//static int current_mode=MODE_TIME;
static int current_mode=MODE_VISUAL;

unsigned char display_buffer[DISPLAY_LINES];

static int i2c_fd=-1;

static int bargraph_i2c(int a, int b, int c) {

	int i;

	char buffer[17];

	buffer[0]=0;

	for(i=0;i<16;i++) buffer[i+1]=0x0;

	/* a */
	if (a>0) {
		a--;
		buffer[1]|=(2<<a)-1;
		if (a>7) buffer[2]|=(2<<(a-8))-1;
	}

	/* b */
	if (b>0) {
		b--;
		buffer[3]|=(2<<b)-1;
		if (b>7) buffer[4]|=(2<<(b-8))-1;
	}

	/* c */
	if (c>0) {
		c--;
		buffer[5]|=(2<<c)-1;
		if (c>7) buffer[6]|=(2<<(c-8))-1;
	}

	if (ioctl(i2c_fd, I2C_SLAVE, HT16K33_ADDRESS0) < 0) {
		fprintf(stderr,"Error setting i2c address %x\n",
			HT16K33_ADDRESS0);
		return -1;
	}

	if ( (write(i2c_fd, buffer, 17)) !=17) {
		fprintf(stderr,"Error writing display %s!\n",
			strerror(errno));
		return -1;
        }

	return 0;
}

static int bargraph_text(int a, int b, int c) {

	int i,j,value;

	for(j=0;j<3;j++) {
		if (j==0) value=a;
		if (j==1) value=b;
		if (j==2) value=c;

		if (value>10) value=10;

		printf("[");
		for(i=0;i<value;i++) printf("*");
		for(i=value;i<10;i++) printf(" ");
		printf("]\n");
	}
	return 0;
}


static int bargraph(int type, int a, int b, int c) {

	if (type&DISPLAY_I2C) {
		bargraph_i2c(a,b,c);
	}

	if (type&DISPLAY_TEXT) {
		bargraph_text(a,b,c);
	}

	return 0;
}


static int close_bargraph(int type) {

	if (type&DISPLAY_I2C) {
		bargraph_i2c(0,0,0);
	}

	return 0;
}




int display_init(int type) {

	int result=0;
	int i;

	if (type&DISPLAY_I2C) {

		i2c_fd=init_i2c("/dev/i2c-1");
		if (i2c_fd < 0) {
			fprintf(stderr,"Error opening device!\n");
			return -1;
 		}

		/* Init display */
		if (init_display(i2c_fd,HT16K33_ADDRESS0,10)) {
			fprintf(stderr,"Error opening display\n");
			return -1;
		}

		/* Init display */
		if (init_display(i2c_fd,HT16K33_ADDRESS2,10)) {
			fprintf(stderr,"Error opening display\n");
			return -1;
		}

		for(i=0;i<DISPLAY_LINES;i++) display_buffer[i]=0;
	}

	return result;
}

static int freq_max[16];
static int freq_matrix[16][8];

#define UPDATE_DIVIDER	5

static int divider=0;

static int freq_display(int display_type, int a, int b, int c) {

	int x,y;
	int i;

	char buffer[17];

	if (a>=0) {
		if (a>15) {
//			printf("A too big %d\n",a);
			a=15;
		}
		freq_max[a]=7;
		for(y=0;y<8;y++) freq_matrix[a][y]=1;
	}

	if (b>=0) {
		if (b>15) {
//			printf("B too big %d\n",b);
			b=15;
		}
		freq_max[b]=7;
		for(y=0;y<8;y++) freq_matrix[b][y]=1;
	}

	if (c>=0) {
		if (c>15) {
//			printf("C too big %d\n",c);
			c=15;
		}
		freq_max[c]=7;
		for(y=0;y<8;y++) freq_matrix[c][y]=1;
	}


	for(x=0;x<16;x++) {
		freq_matrix[x][freq_max[x]]=1;
		if (divider==0)
		if (freq_max[x]>0) freq_max[x]--;
	}

	if (divider==0) 
	if (display_type&DISPLAY_I2C) {
		buffer[0]=0;
		for(i=0;i<16;i++) buffer[i+1]=0x0;

		for(i=0;i<16;i++) {
			for(x=0;x<8;x++) {
				buffer[i+1]|=(freq_matrix[x+(8*(i%2))][i/2]<<x);
			}
		}

		if (ioctl(i2c_fd, I2C_SLAVE, HT16K33_ADDRESS2) < 0) {
			fprintf(stderr,"Error setting i2c address %x\n",
				HT16K33_ADDRESS2);
			return -1;
		}


		if ( (write(i2c_fd, buffer, 17)) !=17) {
			fprintf(stderr,"Error writing display %s!\n",
				strerror(errno));
			return -1;
        	}
	}

	if (divider==0)
	if (display_type&DISPLAY_TEXT) {
		for(y=7;y>=0;y--) {
			for(x=0;x<16;x++) {
				if (freq_matrix[x][y]) printf("*");
				else printf(" ");
			}
			printf("\n");
		}
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



static int close_freq_display(int display_type) {

	int x,y;
	int i;

	char buffer[17];

	/* Clear background */
	for(y=0;y<8;y++) {
		for(x=0;x<16;x++) {
			freq_matrix[x][y]=0;
		}
	}

	if (display_type&DISPLAY_I2C) {

		buffer[0]=0;
		for(i=0;i<16;i++) buffer[i+1]=0x0;

		for(i=0;i<16;i++) {
			for(x=0;x<8;x++) {
				buffer[i+1]|=(freq_matrix[x+(8*(i%2))][i/2]<<x);
			}
		}

		if (ioctl(i2c_fd, I2C_SLAVE, HT16K33_ADDRESS2) < 0) {
			fprintf(stderr,"Error setting i2c address %x\n",
				HT16K33_ADDRESS2);
			return -1;
		}


		if ( (write(i2c_fd, buffer, 17)) !=17) {
			fprintf(stderr,"Error writing display %s!\n",
				strerror(errno));
			return -1;
        	}
	}

	return 0;
}

static int number[10][5][3]={
	{	/* 0 */
		{ 1,1,1,},
		{ 1,0,1,},
		{ 1,0,1,},
		{ 1,0,1,},
		{ 1,1,1,},
	},
	{	/* 1 */
		{ 0,0,1,},
		{ 0,0,1,},
		{ 0,0,1,},
		{ 0,0,1,},
		{ 0,0,1,},
	},
	{	/* 2 */
		{ 1,1,1,},
		{ 0,0,1,},
		{ 1,1,1,},
		{ 1,0,0,},
		{ 1,1,1,},
	},
	{	/* 3 */
		{ 1,1,1,},
		{ 0,0,1,},
		{ 0,1,1,},
		{ 0,0,1,},
		{ 1,1,1,},
	},
	{	/* 4 */
		{ 1,0,1,},
		{ 1,0,1,},
		{ 1,1,1,},
		{ 0,0,1,},
		{ 0,0,1,},
	},
	{	/* 5 */
		{ 1,1,1,},
		{ 1,0,0,},
		{ 1,1,0,},
		{ 0,0,1,},
		{ 1,1,1,},
	},
	{	/* 6 */
		{ 1,1,1,},
		{ 1,0,0,},
		{ 1,1,1,},
		{ 1,0,1,},
		{ 1,1,1,},
	},
	{	/* 7 */
		{ 1,1,1,},
		{ 0,0,1,},
		{ 0,0,1,},
		{ 0,0,1,},
		{ 0,0,1,},
	},
	{	/* 8 */
		{ 1,1,1,},
		{ 1,0,1,},
		{ 1,1,1,},
		{ 1,0,1,},
		{ 1,1,1,},
	},
	{	/* 9 */
		{ 1,1,1,},
		{ 1,0,1,},
		{ 1,1,1,},
		{ 0,0,1,},
		{ 0,0,1,},
	},

};

static int put_number(int which, int x, int y) {

	int a,b;

	for(a=0;a<3;a++) {
		for(b=0;b<5;b++) {
			freq_matrix[a+x][b+y]=number[which][b][a];
		}
	}

	return 0;
}

static int time_display(int display_type, int current_frame, int total_frames) {

	int x,y;
	int i;
	int total_s,current_s,display_m,display_s,bar_length;

	char buffer[17];

	/* Only update a few times a second? */
	/* Should do more for responsiveness? */
	if (current_frame%16!=0) return 0;

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

	if (display_type&DISPLAY_I2C) {

		buffer[0]=0;

		for(i=0;i<16;i++) buffer[i+1]=0x0;

		for(i=0;i<16;i++) {
			for(x=0;x<8;x++) {
				buffer[i+1]|=(freq_matrix[x+(8*(i%2))][i/2]<<x);
			}
		}

		if (ioctl(i2c_fd, I2C_SLAVE, HT16K33_ADDRESS2) < 0) {
			fprintf(stderr,"Error setting i2c address %x\n",
				HT16K33_ADDRESS2);
			return -1;
		}


		if ( (write(i2c_fd, buffer, 17)) !=17) {
			fprintf(stderr,"Error writing display %s!\n",
				strerror(errno));
			return -1;
        	}
	}

	if (display_type&DISPLAY_TEXT) {
		for(y=7;y>=0;y--) {
			for(x=0;x<16;x++) {
				if (freq_matrix[x][y]) printf("*");
				else printf(" ");
			}
			printf("\n");
		}
	}

	return 0;
}

int display_shutdown(int display_type) {

	close_freq_display(display_type);
	close_bargraph(display_type);

	return 0;
}

int display_update(int display_type,
		int aa1, int ba1, int ca1,
		int af1, int bf1, int cf1,
		int current_frame, int num_frames) {

	bargraph(display_type, aa1, ba1, ca1);

	switch(current_mode) {
		case MODE_VISUAL:
			freq_display(display_type, af1, bf1, cf1);
			break;
		case MODE_TIME:
			time_display(display_type, current_frame, num_frames);
			break;
		default:
			printf("Unknown visual mode!\n");
			break;
	}

	return 0;
}
