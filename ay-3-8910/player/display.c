/* Makes a spinning pattern on the display */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "i2c_lib.h"

#include "display.h"

unsigned char display_buffer[DISPLAY_LINES];

static int i2c_fd=-1;

static int bargraph_i2c(int which, int value) {

	int i;

	char buffer[17];

	buffer[0]=0;

	for(i=0;i<16;i++) buffer[i+1]=0xff;

	if ( (write(i2c_fd, buffer, 17)) !=17) {
		fprintf(stderr,"Erorr writing display!\n");
		return -1;
        }

	return 0;
}

static int bargraph_text(int value) {

	int i;

	if (value>10) value=10;

	printf("[");
	for(i=0;i<value;i++) printf("*");
	for(i=value;i<10;i++) printf(" ");
	printf("]\n");
	return 0;
}


int bargraph(int type, int which, int value) {

	if (type&DISPLAY_I2C) {
		bargraph_i2c(which, value);
	}

	if (type&DISPLAY_TEXT) {
		bargraph_text(value);
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
