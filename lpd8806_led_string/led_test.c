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

	int brightness=0,direction=1;

	while(1) {

		brightness+=direction;
		if (brightness>(MAX_BRIGHTNESS-2)) direction=-1;
		if (brightness<1) direction=1;

		data[0] = 128;
		data[1] = 128+brightness;
		data[2] = 128;
	
		data[3] = 128+((MAX_BRIGHTNESS-1)-brightness);
		data[4] = 128;
		data[5] = 128;



		data[6] = 128;
		data[7] = 128;
		data[8] = 128+brightness;

		for(i=0;i<128;i++) {
			result=write(spi_fd,&data[i],1);
			if (result<1) {
				printf("error!\n");
				exit(-1);
			}
		}
		printf("%d\n",brightness);

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
