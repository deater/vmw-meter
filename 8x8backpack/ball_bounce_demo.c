/* Makes a spinning pattern on the display */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "i2c_lib.h"
#include "graphics_lib.h"


/* should make the device settable */
int ball_bounce_demo(int i2c_fd) {

	int i;
	double x,y,xadd,yadd;
	unsigned char display_buffer[DISPLAY_LINES];

	x=2.0, y=5.0;

	xadd=1.2; yadd=0.8;

	while(1) {
		for(i=0;i<DISPLAY_LINES;i++) display_buffer[i]=0;

		x+=xadd;
		if (x<0.0) {
			x=0.0;
			xadd=-xadd;
		}
		if (x>7.0) {
			x=7.0;
			xadd=-xadd;
		}

		y+=yadd;
		if (y<0.0) {
			y=0.0;
			yadd=-yadd;
		}
		if (y>7.0) {
			y=7.0;
			yadd=-yadd;
		}

		plotxy(display_buffer,(int)x,(int)y);
		update_8x8_display_rotated(i2c_fd,HT16K33_ADDRESS0,display_buffer,0);
		usleep(100000);
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
        if (init_display(i2c_fd,HT16K33_ADDRESS0,10)) {
                fprintf(stderr,"Error opening display\n");
                return -1;
        }

	result=ball_bounce_demo(i2c_fd);

	return result;
}

