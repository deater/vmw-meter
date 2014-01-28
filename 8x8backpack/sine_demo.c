/* Makes a sine wave */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <math.h>

#include "i2c_lib.h"
#include "graphics_lib.h"

#define XSIZE 8
#define YSIZE 8

#define PI 3.14159265358979323846264338327950

/* each possible display */
double dx[256];

int sine_demo_init(int i2c_fd, int device_num, int brightness,
			double phase) {

	/* Init display */
	if (init_display(i2c_fd,device_num,brightness)) {
		fprintf(stderr,"Error opening display\n");
		return -1;
	}

	dx[device_num]=phase;

	return 0;
}


/* Draw one frame */
/* dx is the only data needed to be saved across calls */
int sine_demo(int i2c_fd, int device_num) {

	int x,y;
	double temp_dx,dy,dxscroll=PI/8.0;

	int out_buffer[XSIZE][YSIZE];

	unsigned char display_buffer[DISPLAY_LINES];

	for(x=0;x<XSIZE;x++) for (y=0;y<YSIZE;y++) out_buffer[x][y]=0;

	for(x=0;x<XSIZE;x++) {
		temp_dx=dx[device_num]+((double)x *(PI/8.0));
		dy=sin(temp_dx);
		y=(int)round((3.5+(dy*3.5)));
		out_buffer[x][y]=1;
//		printf("%d %d %lf %lf\n",x,y,temp_dx,dy);
	}

	/* Put scroll buffer into output buffer */
	for(y=0;y<YSIZE;y++) {
		/* clear the line before drawing to it */
		display_buffer[y]=0;
		for(x=0;x<XSIZE;x++) {
			if (out_buffer[x][y]) {
				plotxy(display_buffer,x,y);
			}
		}
	}

	update_8x8_display_rotated(i2c_fd,device_num,display_buffer,0);

	dx[device_num]+=dxscroll;

	return 0;
}


int main(int argc, char **argv) {

	int result;

	int i2c_fd;
	int available[256];

	i2c_fd=init_i2c("/dev/i2c-1");
	if (i2c_fd < 0) {
		fprintf(stderr,"Error opening device!\n");
		return -1;
	}

	/* Init display */
	available[HT16K33_ADDRESS0]=1;
	if (sine_demo_init(i2c_fd,HT16K33_ADDRESS0,10,0.0)) {
		fprintf(stderr,"Error opening display 1\n");
		available[HT16K33_ADDRESS0]=0;
	}

	/* Init display */
	available[HT16K33_ADDRESS1]=1;
	if (sine_demo_init(i2c_fd,HT16K33_ADDRESS1,10,90.0)) {
		fprintf(stderr,"Error opening display\n");
		available[HT16K33_ADDRESS1]=0;
	}


	while(1) {
		if (available[HT16K33_ADDRESS0]) {
			sine_demo(i2c_fd,HT16K33_ADDRESS0);
		}
		if (available[HT16K33_ADDRESS1]) {
			sine_demo(i2c_fd,HT16K33_ADDRESS1);
		}
		usleep(50000);
	}

	return result;
}

