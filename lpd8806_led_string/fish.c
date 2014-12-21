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

#define MAX_FISH	10

	struct fish_type {
		int x;
		int speed;
		int r,g,b;
		int out;
	} fishes[MAX_FISH];

	for(i=0;i<MAX_FISH;i++) {
		fishes[i].out=0;
	}

	while(1) {

		if (random()%40) {
			for(i=0;i<MAX_FISH;i++) {
				if (fishes[i].out==0) {
					fishes[i].out=1;
					fishes[i].speed=(random()%2)+1;
					if (random()%2) fishes[i].speed=-fishes[i].speed;

					if (fishes[i].speed>0) {
 						fishes[i].x=0;
					}
					else {
						fishes[i].x=31;
					}
					int color=random()%MAX_COLORS;
					fishes[i].r=colors[color].r;
					fishes[i].g=colors[color].g;
					fishes[i].b=colors[color].b;

					break;
				}
			}
		}

		for(i=0;i<32;i++) {
			data[(i*3)]=128;
			data[(i*3)+1]=128;
			data[(i*3)+2]=128;
		}



		for(i=0;i<MAX_FISH;i++) {
			if (fishes[i].out) {
				data[(fishes[i].x*3)]=128+fishes[i].g;
				data[(fishes[i].x*3)+1]=128+fishes[i].r;
				data[(fishes[i].x*3)+2]=128+fishes[i].b;

				fishes[i].x+=fishes[i].speed;
				if (fishes[i].x<0) fishes[i].out=0;
				if (fishes[i].x>31) fishes[i].out=0;
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
		usleep(100000);

	}

	spi_close(spi_fd);

	return 0;
}
