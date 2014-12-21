#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <linux/spi/spidev.h>

#include "spi_lib.h"

#include "colors.h"


int main(int argc, char **argv) {

	int spi_fd,i;
	unsigned char zeros[128],data[128];
	int result;
	int r,g,b;

	spi_fd=spi_open("/dev/spidev0.0", SPI_MODE_0, 100000, 8);
	if (spi_fd<0) {
		exit(-1);
	}

	/* Send a byte acting as a start bit */

	for(i=0;i<128;i++) zeros[i]=0;
	for(i=0;i<128;i++) data[i]=128;

	for(i=0;i<128;i++) {
		result=write(spi_fd,&zeros[i],1);
		if (result<1) {
			printf("error!\n");
			exit(-1);
		}
	}

#define MAX_BRIGHTNESS 64

	int location=0,direction=1;
	int c=0;

	c=random()%MAX_COLORS;

	while(1) {

		r=colors[c].r;
		g=colors[c].g;
		b=colors[c].b;


		location+=direction;
		if (location>31) {
			direction=-1;
			c=random()%MAX_COLORS;
		}

		if (location<1) {
			direction=1;
			c=random()%MAX_COLORS;
		}

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

		for(i=0;i<128;i++) {
			result=write(spi_fd,&data[i],1);
			if (result<1) {
				printf("error!\n");
				exit(-1);
			}
		}

		for(i=0;i<128;i++) {
			result=write(spi_fd,&zeros[i],1);
			if (result<1) {
				printf("error!\n");
				exit(-1);
			}
		}

	}

	spi_close(spi_fd);

	return 0;
}
