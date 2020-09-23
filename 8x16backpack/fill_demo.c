/* Makes a spinning pattern on the display */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "i2c_lib.h"

/* should make the device settable */
int fill_demo(int i2c_fd) {

	int i,x,y;
	unsigned short display_buffer[DISPLAY_LINES];

	while(1) {
		for(i=0;i<DISPLAY_LINES;i++) display_buffer[i]=0;

		for(y=0;y<DISPLAY_LINES;y++) {
			for(x=0;x<16;x++) {

				usleep(50000);

				/* write out to hardware */
				display_buffer[y]<<=1;
				display_buffer[y]+=1;
				update_8x16_display(i2c_fd,
					HT16K33_ADDRESS0,display_buffer,
					0,BROKEN);
			}
		}
		usleep(500000);
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

	result=fill_demo(i2c_fd);

	return result;
}

