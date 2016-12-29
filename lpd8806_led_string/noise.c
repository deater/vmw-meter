#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "lpd8806.h"
#include "effects.h"

#define MAX_BRIGHTNESS 64

int noise(int spi_fd) {

	int i;
	unsigned char data[128];

	/* Set data to all black */
	for(i=0;i<128;i++) data[i]=128;

	while(1) {

		for(i=0;i<32;i++) {
			data[(i*3)]=128+random()%MAX_BRIGHTNESS;
			data[(i*3)+1]=128+random()%MAX_BRIGHTNESS;
			data[(i*3)+2]=128+random()%MAX_BRIGHTNESS;

		}

		lpd8806_write(spi_fd,data);

		usleep(200000);

	}

	lpd8806_close(spi_fd);

	return 0;
}
