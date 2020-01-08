/* Makes a spinning pattern on the display */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "i2c_lib.h"
#include "default_device.h"

int main(int argc, char **argv) {

	int i,spin=0,dot1=0,dot2=19,blob=0;
	unsigned short display_state[8];
	int meter_fd,display_present;

	display_present=1;
	meter_fd=init_i2c(DEFAULT_DEVICE);
	if (meter_fd < 0) {
		fprintf(stderr,"Error opening device %s!\n",DEFAULT_DEVICE);
		display_present=0;
	}

	if (display_present) {
		init_saa1064_display(meter_fd,SAA1064_ADDRESS1);
		init_saa1064_display(meter_fd,SAA1064_ADDRESS2);
		init_saa1064_display(meter_fd,SAA1064_ADDRESS3);
		init_saa1064_display(meter_fd,SAA1064_ADDRESS4);
	}

	while(1) {
		for(i=0;i<6;i++) {
			display_state[i]=0xfc00|(0x0200>>spin);
		}
		spin++;
		if (spin>7) spin=0;

		display_state[6]=0;
		display_state[7]=0;

		if (dot1<16) display_state[6]|=1<<dot1;
		else if (dot1<22) display_state[dot1-16]|=SAA1064_SEGMENT_DP;
		else display_state[7]=1<<(15-(dot1-22));

		if (dot2<16) display_state[6]|=1<<dot2;
		else if (dot2<22) display_state[dot2-16]|=SAA1064_SEGMENT_DP;
		else display_state[7]=1<<(15-(dot2-22));

		dot1++;
		dot2++;
		if (dot1>37) dot1=0;
		if (dot2>37) dot2=0;

		if (spin%2) blob=rand();
		for(i=0;i<6;i++) {
			if (blob&1<<i) display_state[i]|=SAA1064_SEGMENT_EX;
		}

		if (display_present) {
			update_saa1064_display(meter_fd, SAA1064_ADDRESS1,
				display_state);
		}
		else {
			update_saa1064_ascii(display_state);
		}

		usleep(100000);
	}


	return 0;
}


