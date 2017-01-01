#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "lpd8806.h"
#include "effects.h"
#include "colors.h"

int scanner_dual(int spi_fd, char *color1, char *color2) {

	int i;
	unsigned char data[128];

	int s1_location=31,s1_direction=1;
	int s1_r,s1_g,s1_b;

	int s2_location=0,s2_direction=1;
	int s2_r,s2_g,s2_b;

	printf("Running scanner_dual %s %s\n",color1,color2);

	if (color1) {
		get_color(color1,&s1_r,&s1_g,&s1_b);
	} else {
		s1_r=0;
		s1_g=64;
		s1_b=0;
	}

	if (color2) {
		get_color(color2,&s2_r,&s2_g,&s2_b);
	} else {
		s2_r=64;
		s2_g=0;
		s2_b=0;
	}


	/* Clear out data */
	for(i=0;i<128;i++) data[i]=128;

	while(1) {

		s1_location+=s1_direction;
		if (s1_location>31) s1_direction=-1;
		if (s1_location<1) s1_direction=1;

		s2_location+=s2_direction;
		if (s2_location>31) s2_direction=-1;
		if (s2_location<1) s2_direction=1;

		for(i=0;i<32;i++) {

			/* clear to black */
			data[(i*3)+0]=128;
			data[(i*3)+1]=128;
			data[(i*3)+2]=128;

			/* s2 */
			if (i==s2_location) {
				data[(i*3)]=128+s2_g;
				data[(i*3)+1]=128+s2_r;
				data[(i*3)+2]=128+s2_b;
			}
			else if ((i==s2_location-1) || (i==s2_location+1)) {
				data[(i*3)]=128+(s2_g/8);
				data[(i*3)+1]=128+(s2_r/8);
				data[(i*3)+2]=128+(s2_b/8);
			}
			else if( (i==s2_location-2) || (i==s2_location+2)) {
				data[(i*3)]=128+(s2_g/32);
				data[(i*3)+1]=128+(s2_r/32);
				data[(i*3)+2]=128+(s2_b/32);
			}
			else {
//				data[(i*3)]=128;
//				data[(i*3)+1]=128;
//				data[(i*3)+2]=128;
			}

			/* s1 */

			if (i==s1_location) {
				data[(i*3)+0]=128+s1_g;
				data[(i*3)+1]=128+s1_r;
				data[(i*3)+2]=128+s1_b;
			}
			else

			if ((i==s1_location-1) || (i==s1_location+1)) {
				data[(i*3)+0]=128+(s1_g/8);
				data[(i*3)+1]=128+(s1_r/8);
				data[(i*3)+2]=128+(s1_b/8);
			}

			else if( (i==s1_location-2) || (i==s1_location+2)) {
				data[(i*3)+0]=128+(s1_g/32);
				data[(i*3)+1]=128+(s1_r/32);
				data[(i*3)+2]=128+(s1_b/32);
			}
			else {
//				data[(i*3)+0]=128;
//				data[(i*3)+1]=128;
//				data[(i*3)+2]=128;
			}

		}

		lpd8806_write(spi_fd,data);

	}

	lpd8806_close(spi_fd);

	return 0;
}
