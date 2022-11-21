/* This tool lets you set all of the segments on the display */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include "14seg_font.h"
#include "i2c_lib.h"
#include "default_device.h"

int main(int argc, char **argv) {

	int i;
	unsigned short display_state[8];
	char string[256],*endptr;
	int top,dp,left,right;
	int meter_fd,display_present;

	if (argc<6) {
		printf("Usage: %s string top_leds decimal_points left_bar right_bar\n",
			argv[0]);
		exit(1);
	}

	strcpy(string,argv[1]);

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


	/* set string */
	for(i=0;i<6;i++) {
		if (i<strlen(string)) {
			display_state[i]=ascii_lookup[(unsigned char)string[i]];
		}
		else display_state[i]=0;
	}

	/* top */
	top=strtol(argv[2], &endptr, 0);
	for(i=0;i<6;i++) {
		if (top&(1<<(5-i))) display_state[i]|=SAA1064_SEGMENT_EX;
	}

	/* dp */
	dp=strtol(argv[3], &endptr, 0);
	for(i=0;i<6;i++) {
		if (dp&(1<<(5-i))) display_state[i]|=SAA1064_SEGMENT_DP;
	}

	/* left */
	left=strtol(argv[4], &endptr, 0);
	display_state[6]=left;

	/* right */
	right=strtol(argv[5], &endptr, 0);
	display_state[7]=right;

	/* update the display */

	if (display_present) {
		update_saa1064_display(meter_fd, SAA1064_ADDRESS1,
			display_state);
	}
	else {
		update_saa1064_ascii(display_state);
	}

  	return 0;
}


