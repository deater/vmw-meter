/* Makes a spinning pattern on the display */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "i2c_lib.h"

unsigned char display_buffer[DISPLAY_LINES];


int bargraph(int i2c_fd, int which, int value) {

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


int main(int argc, char **argv) {

	int result;

	int i2c_fd,i;

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

	while(1) {
		for(i=0;i<10;i++) {
			bargraph(i2c_fd,0,i);
			bargraph(i2c_fd,1,10-i);
			bargraph(i2c_fd,2,i);
			usleep(500000);
		}
	}

	return result;
}

