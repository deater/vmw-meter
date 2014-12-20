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

	int g_location=0,g_direction=1;
	int r_location=31,r_direction=1;

	while(1) {

		r_location+=r_direction;
		if (r_location>31) r_direction=-1;
		if (r_location<1) r_direction=1;

		g_location+=g_direction;
		if (g_location>31) g_direction=-1;
		if (g_location<1) g_direction=1;


		for(i=0;i<32;i++) {
			data[(i*3)+0]=128;
			data[(i*3)+1]=128;
			data[(i*3)+2]=128;


			/* g */
			if (i==g_location) {
				data[(i*3)]=128+64;
			}
			else

			if ((i==g_location-1) || (i==g_location+1)) {
				data[(i*3)]=128+8;
			}

			else if( (i==g_location-2) || (i==g_location+2)) {
				data[(i*3)]=128+2;
			}
			else {
				data[(i*3)]=128;
			}

			/* r */

			if (i==r_location) {
				data[(i*3)+1]=128+64;
			}
			else

			if ((i==r_location-1) || (i==r_location+1)) {
				data[(i*3)+1]=128+8;
			}

			else if( (i==r_location-2) || (i==r_location+2)) {
				data[(i*3)+1]=128+2;
			}
			else {
				data[(i*3)+1]=128;
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
