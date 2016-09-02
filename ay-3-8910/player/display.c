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

#include "i2c_lib.h"

#include "display.h"

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


int bargraph(int type, int a, int b, int c) {

	if (type&DISPLAY_I2C) {
		bargraph_i2c(a,b,c);
	}

	if (type&DISPLAY_TEXT) {
		bargraph_text(a,b,c);
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

		for(i=0;i<DISPLAY_LINES;i++) display_buffer[i]=0;
	}

	return result;
}

static int freq_max[16];

static int freq_matrix[16][8];

int freq_display(int a, int b, int c) {

	int x,y,a_x,b_x,c_x;

	for(y=0;y<8;y++) {
		for(x=0;x<16;x++) {
			freq_matrix[x][y]=0;
		}
	}

	a_x=(a*16)/0xfff;
	b_x=(b*16)/0xfff;
	c_x=(c*16)/0xfff;

	if (a_x>15) {
		printf("A too big %x %d\n",a,a_x);
		exit(1);
	}
	if (b_x>15) {
		printf("B too big %x %d\n",b,b_x);
		exit(1);
	}
	if (c_x>15) {
		printf("C too big %x %d\n",c,c_x);
		exit(1);
	}

	for(x=0;x<16;x++) {
		freq_matrix[x][freq_max[x]/10]=1;
		if (freq_max[x]>0) freq_max[x]--;
	}

	freq_max[a_x]=79;
	freq_max[b_x]=79;
	freq_max[c_x]=79;


	for(y=0;y<8;y++) freq_matrix[a_x][y]=1;
	for(y=0;y<8;y++) freq_matrix[b_x][y]=1;
	for(y=0;y<8;y++) freq_matrix[c_x][y]=1;

	for(y=7;y>=0;y--) {
		for(x=0;x<16;x++) {
			if (freq_matrix[x][y]) printf("*");
			else printf(" ");
		}
		printf("\n");
	}
	return 0;
}
