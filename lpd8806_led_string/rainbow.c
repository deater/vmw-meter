#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "effects.h"

#include "lpd8806.h"

//#define MAX_BRIGHTNESS 64

int rainbow(int spi_fd) {

	int i,j;
	unsigned char data[128];
	int location=0;

	/* Set data to all black */
	for(i=0;i<128;i++) data[i]=128;

	while(1) {

		location++;
		if (location>=7) location=0;

		for(i=0;i<32;i++) {
			j=i+location;

			/* red */
			if (j%7==0) {
				data[(i*3)]=128;
				data[(i*3)+1]=128+64;
				data[(i*3)+2]=128;
			}

			/* orange */
			if (j%7==1) {
				data[(i*3)]=128+25;
				data[(i*3)+1]=128+64;
				data[(i*3)+2]=128;
			}

			/* yellow */
			if (j%7==2) {
				data[(i*3)]=128+64;
				data[(i*3)+1]=128+64;
				data[(i*3)+2]=128;
			}

			/* green */
			if (j%7==3) {
				data[(i*3)]=128+64;
				data[(i*3)+1]=128;
				data[(i*3)+2]=128;
			}

			/* blue */
			if (j%7==4) {
				data[(i*3)]=128;
				data[(i*3)+1]=128;
				data[(i*3)+2]=128+64;
			}
			/* indigo */
			if (j%7==5) {
				data[(i*3)]=128;
				data[(i*3)+1]=128+19;
				data[(i*3)+2]=128+32;
			}
			/* violet */
			if (j%7==6) {
				data[(i*3)]=128;
				data[(i*3)+1]=128+25;
				data[(i*3)+2]=128+25;
			}

		}

		lpd8806_write(spi_fd, data);

		usleep(200000);

	}

	return 0;
}
