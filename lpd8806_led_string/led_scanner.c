#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <linux/spi/spidev.h>

#include "spi_lib.h"



int main(int argc, char **argv) {

	int spi_fd,i;
	unsigned char zeros[128],data[128];
	int result;

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

	while(1) {

		location+=direction;
		if (location>31) direction=-1;
		if (location<1) direction=1;

		for(i=0;i<32;i++) {
			if (i==location) {
				data[(i*3)]=128+64;
			}
			else

			if ((i==location-1) || (i==location+1)) {
				data[(i*3)]=128+8;
			}

			else if( (i==location-2) || (i==location+2)) {
				data[(i*3)]=128+2;
			}


			else {
				data[(i*3)]=128;
			}
			data[(i*3)+1]=128;
			data[(i*3)+2]=128;
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
