#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <math.h>

#include "effects.h"
#include "lpd8806.h"

#define MAX_BRIGHTNESS 64

int stars(int spi_fd, char *speed_s, char *dimming_s) {

	int i;
	unsigned char data[128];
	int speed,dimming;
	int star_num;

	if (speed_s) {
		speed=atoi(speed_s);
	} else {
		speed=20;
	}


	if (dimming_s) {
		dimming=atoi(dimming_s);
	} else {
		dimming=3;
	}

	for(i=0;i<128;i++) data[i]=128;

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

		lpd8806_write(spi_fd, data);

		usleep(1000);

	}

	return 0;
}
