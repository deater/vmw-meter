/* Makes a pulsing pattern on the display */

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "i2c_lib.h"

int main(int argc, char **argv) {

	int i,pulse1=0,pulsedir1=1,pulse2=15,pulsedir2=-1,count=0;
	unsigned short display_state[8];
	int meter_fd,display_present;

	display_present=1;
	meter_fd=init_i2c("/dev/i2c-6");
	if (meter_fd < 0) {
		fprintf(stderr,"Error opening device!\n");
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
			if (count&0x4) display_state[i]=0xaaaa;
			else display_state[i]=0x5555;
		}
		display_state[6]=~((1<<pulse1)-1);
		display_state[7]=~((1<<pulse2)-1);

		if (display_present) {
			update_saa1064_display(meter_fd, SAA1064_ADDRESS1,
				display_state);
		}
		else {
			update_saa1064_ascii(display_state);
		}

		usleep(100000);

		count++;

		pulse1=pulse1+pulsedir1;
		if ((pulse1>15) || (pulse1<1)) pulsedir1=-pulsedir1;
		pulse2=pulse2+pulsedir2;
		if ((pulse2>15) || (pulse2<1)) pulsedir2=-pulsedir2;

	}


	return 0;
}


