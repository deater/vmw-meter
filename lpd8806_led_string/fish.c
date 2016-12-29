#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "lpd8806.h"
#include "effects.h"
#include "colors.h"

#define MAX_FISH	10

struct fish_type {
	int x;
	int speed;
	int r,g,b;
	int out;
} fishes[MAX_FISH];

int fish(int spi_fd, char *nfish) {

	int i;
	unsigned char data[128];
	int num_fish,color;

	if (nfish) {
		num_fish=atoi(nfish);
		if (num_fish>MAX_FISH) num_fish=MAX_FISH;
	}
	else {
		num_fish=MAX_FISH;
	}

	printf("Running fish with %d fishes\n",num_fish);

	/* clear data */
	for(i=0;i<128;i++) data[i]=128;

	for(i=0;i<num_fish;i++) {
		fishes[i].out=0;
	}

	while(1) {

		if (random()%40) {
			for(i=0;i<num_fish;i++) {
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
					color=random()%MAX_COLORS;
					fishes[i].r=colors[color].r;
					fishes[i].g=colors[color].g;
					fishes[i].b=colors[color].b;

					break;
				}
			}
		}

		/* clear to black */
		for(i=0;i<32;i++) {
			data[(i*3)]=128;
			data[(i*3)+1]=128;
			data[(i*3)+2]=128;
		}

		for(i=0;i<num_fish;i++) {
			if (fishes[i].out) {
				data[(fishes[i].x*3)]=128+fishes[i].g;
				data[(fishes[i].x*3)+1]=128+fishes[i].r;
				data[(fishes[i].x*3)+2]=128+fishes[i].b;

				fishes[i].x+=fishes[i].speed;
				if (fishes[i].x<0) fishes[i].out=0;
				if (fishes[i].x>31) fishes[i].out=0;
			}

		}

		lpd8806_write(spi_fd,data);

		usleep(100000);

	}

	lpd8806_close(spi_fd);

	return 0;
}
