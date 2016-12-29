#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "lpd8806.h"
#include "effects.h"
#include "colors.h"


int scanner_blinky(int spi_fd) {

	int i;
	unsigned char data[128];
	int r,g,b;
	int location=0,direction=1;
	int c=0;

	/* clear data */
	for(i=0;i<128;i++) data[i]=128;


	while(1) {

		c=random()%MAX_COLORS;
		r=colors[c].r;
		g=colors[c].g;
		b=colors[c].b;


		location+=direction;
		if (location>31) direction=-1;
		if (location<1) direction=1;

		for(i=0;i<32;i++) {
			if (i==location) {
				data[(i*3)]=128+g;
				data[(i*3)+1]=128+r;
				data[(i*3)+2]=128+b;
			}
			else

			if ((i==location-1) || (i==location+1)) {
				data[(i*3)]=128+(g/8);
				data[(i*3)+1]=128+(r/8);
				data[(i*3)+2]=128+(b/8);
			}

			else if( (i==location-2) || (i==location+2)) {
				data[(i*3)]=128+(g/32);
				data[(i*3)+1]=128+(r/32);
				data[(i*3)+2]=128+(b/32);
			}


			else {
				data[(i*3)]=128;
				data[(i*3)+1]=128;
				data[(i*3)+2]=128;
			}
		}

		lpd8806_write(spi_fd,data);

	}

	lpd8806_close(spi_fd);

	return 0;
}
