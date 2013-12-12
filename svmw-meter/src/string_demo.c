/* This tool will repeatedly prompt for a string which is then */
/* printed on the display.                                     */

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "14seg_font.h"
#include "i2c_lib.h"

int main(int argc, char **argv) {

	int i;
	unsigned short display_state[8];
	char string[256];
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

	for(i=0;i<8;i++) display_state[i]=0;

	while(1) {
		if (fgets(string,256,stdin)==NULL) break;
		puts(string);

		for(i=0;i<6;i++) {
			if (i<strlen(string)-1) {
				display_state[i]=ascii_lookup[(unsigned char)string[i]];
			}
			else display_state[i]=0;
		}
		if (display_present) {
			update_saa1064_display(meter_fd, SAA1064_ADDRESS1,
				display_state);
		}
		else {
			update_saa1064_ascii(display_state);
		}
	}

	return 0;
}


