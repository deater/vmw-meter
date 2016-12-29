#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "lpd8806.h"
#include "effects.h"
#include "colors.h"

#define MAX_BRIGHTNESS 64

int gradient(int spi_fd, char *color) {

	int i;
	unsigned char data[128];

	/* clear out data */
	for(i=0;i<128;i++) data[i]=128;

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

		lpd8806_write(spi_fd,data);
	}

	lpd8806_close(spi_fd);

	return 0;
}
