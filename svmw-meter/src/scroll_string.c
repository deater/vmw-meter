/* scroll a string on the display */
/* cat README | ./scroll_string.ascii */
/* will display text file README to the display */

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "14seg_font.h"
#include "i2c_lib.h"
#include "default_device.h"

int main(int argc, char **argv) {

	int i,len=0,pos=0;
	unsigned short display_state[8];
	char string[256];

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
		if (fgets(string,256,stdin)==NULL) break;

		len=strlen(string);
		pos=0;

		while(1) {

			for(i=0;i<6;i++) {
				if (pos+i<len-1) {
					display_state[i]=ascii_lookup[(unsigned char)string[pos+i]];
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

			/* pause longer if first char */
			if (pos==0) usleep(500000);
			usleep(500000);

			pos++;
			if (pos==len) break;
		}

	}

	return 0;
}


