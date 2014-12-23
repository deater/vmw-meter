#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <linux/spi/spidev.h>

#include <math.h>

#include "spi_lib.h"

#include "colors.h"


int main(int argc, char **argv) {

	int spi_fd,i;
	unsigned char zeros[128],data[128];
	int result;
	int speed,dimming;

	if (argc>1) {
		speed=atoi(argv[1]);
	} else {
		speed=20;
	}


	if (argc>1) {
		dimming=atoi(argv[2]);
	} else {
		dimming=3;
	}



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

	int star_num;

	while(1) {
		if (random()%speed==0) {
			star_num=random()%32;

			data[(star_num*3)+0]=128+MAX_BRIGHTNESS;
			data[(star_num*3)+1]=128+MAX_BRIGHTNESS;
			data[(star_num*3)+2]=128+MAX_BRIGHTNESS;
		}


		for(i=0;i<32*3;i++) {
			if (data[i]>128) data[i]-=dimming;
			else data[i]=128;
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
		usleep(1000);

	}

	spi_close(spi_fd);

	return 0;
}
