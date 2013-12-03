/* Makes a starfield demo */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <math.h>

#include "i2c_lib.h"
#include "graphics_lib.h"

#define XSIZE 8
#define YSIZE 8

#define SPACE_XSIZE 1024
#define SPACE_YSIZE 1024
#define X_RANGE SPACE_XSIZE/2
#define Y_RANGE SPACE_YSIZE/2

#define ZSCALE 64.0
#define ZSCALE_HALF ZSCALE/2
#define ZSPEED 0.5

#define NUM_STARS 10

struct star_info {
   double start_x;
   double start_y;
   int x;
   int y;
   double z;
   int active;
} stars[NUM_STARS];

int create_stars(void) {

	int i;

	for(i=0;i<NUM_STARS;i++) {

		if (stars[i].active<0) stars[i].active++;

		if (stars[i].active==0) {
			stars[i].z=(rand()%(int)ZSCALE)+ZSCALE/2;
			stars[i].start_x=( (rand()%SPACE_XSIZE)/4)-(X_RANGE/4);
			stars[i].start_y=( (rand()%SPACE_YSIZE)/4)-(Y_RANGE/4);
			stars[i].active=1;
//			printf("New star at %d, %lf %lf\n",i,stars[i].start_x,stars[i].start_y);
		}
	}

	return 0;
}

int starfield_demo(int i2c_fd) {

	int i,y,newx,newy;
	int odd=0;

	unsigned char display_buffer[DISPLAY_LINES];

	for(i=0;i<NUM_STARS;i++) stars[i].active=0;

	create_stars();

	while(1) {
		for (y=0;y<YSIZE;y++) display_buffer[y]=0;

		for(i=0;i<NUM_STARS;i++) {
			if (stars[i].active>0) {
//				if (stars[i].z>ZSCALE_HALF) {
//					if (!odd) {
//						plotxy(display_buffer,stars[i].x,stars[i].y);
//					}
//				} else {
					plotxy(display_buffer,stars[i].x,stars[i].y);
//				}
			}
		}

		update_8x8_display_rotated(i2c_fd,HT16K33_ADDRESS1,display_buffer,0);

		/* create new stars if any off screen */

		create_stars();

		/* Move stars */
		for(i=0;i<NUM_STARS;i++) {

			if (stars[i].active<=0) continue;

			newx=(((ZSCALE* stars[i].start_x) / stars[i].z) + X_RANGE) / (SPACE_XSIZE/XSIZE);
			newy=(((ZSCALE* stars[i].start_y) / stars[i].z) + Y_RANGE) / (SPACE_XSIZE/XSIZE);
//			printf("%d: %d %d -> %d %d\n",i,stars[i].x,stars[i].y,newx,newy);

			if ((newx>=XSIZE) || (newx<0) || (newy>=YSIZE) || (newy<0)) {
				stars[i].x=0; stars[i].y=0; stars[i].z=0;
				stars[i].active=-5;
			}
			else {
				stars[i].x=newx;
				stars[i].y=newy;
			}
			stars[i].z-=ZSPEED;
			if (stars[i].z<=0) {
				stars[i].active=-1;
			}
		}
		odd=!odd;
		usleep(10000);
	}

	return 0;
}


int main(int argc, char **argv) {

	int result;
	int i2c_fd;

	i2c_fd=init_i2c("/dev/i2c-1");
	if (i2c_fd < 0) {
		fprintf(stderr,"Error opening device!\n");
		return -1;
	}

	/* Init display */
	if (init_display(i2c_fd,HT16K33_ADDRESS1,10)) {
		fprintf(stderr,"Error opening display\n");
		return -1;
	}

	result=starfield_demo(i2c_fd);

	return result;
}

