#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "lpd8806.h"
#include "effects.h"
#include "colors.h"

#define MAX_BRIGHTNESS 16

int gradient(int spi_fd, char *color) {

	int i;
	unsigned char data[128];
	int r,g,b;

	/* clear out data */
	for(i=0;i<128;i++) data[i]=128;

	int brightness=0,direction=1;

	if (color) {
		get_color(color,&r,&g,&b);
	}
	else {
		r=64;
		g=0;
		b=0;
	}

	while(1) {

//		printf("%d\n",brightness);

		brightness+=direction;
		if (brightness>(MAX_BRIGHTNESS-2)) direction=-1;
		if (brightness<2) direction=1;

		for(i=0;i<32;i++) {

			data[(i*3)] = 128+(g/brightness);
			data[(i*3)+1] = 128+(r/brightness);
			data[(i*3)+2] = 128+(b/brightness);
		}

		lpd8806_write(spi_fd,data);
	}

	lpd8806_close(spi_fd);

	return 0;
}
