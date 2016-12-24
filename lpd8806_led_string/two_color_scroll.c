#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "colors.h"
#include "effects.h"
#include "lpd8806.h"

#define MAX_BRIGHTNESS 64

int two_color_scroll(int spi_fd, char *col1, char *col2, char *dir) {

	int i,j;
	unsigned char data[128];
	int location=0;
	int direction=0;
	int r1,g1,b1,r2,g2,b2;

	printf("two_color_scroll %s %s %s\n",col1,col2,dir);

	if (dir) direction=atoi(dir);
	if (col1) get_color(col1, &r1,&g1,&b1);
	if (col2) get_color(col2, &r2,&g2,&b2);

	/* Set data to all black */
	for(i=0;i<128;i++) data[i]=128;

	while(1) {

		if (direction==0) {
			location++;
			if (location>=8) location=0;
		}
		else {
			location--;
			if (location>=8) location=0;
			if (location<1) location=7;
		}

		for(i=0;i<32;i++) {
			j=i+location;
			if (j%8==0) {
				data[(i*3)]=128+r1;	/* Bright Red */
				data[(i*3)+1]=128+g1;
				data[(i*3)+2]=128+b1;
			}

			if (j%8==1) {
				data[(i*3)]=128+r1/4;	/* Less Bright Red */
				data[(i*3)+1]=128+g1/4;
				data[(i*3)+2]=128+b1/4;
			}

			if (j%8==2) {
				data[(i*3)]=128+r2/4;	/* Less Bright Green */
				data[(i*3)+1]=128+g2/4;
				data[(i*3)+2]=128+b2/4;
			}

			if (j%8==3) {
				data[(i*3)]=128+r2;	/* Bright Green */
				data[(i*3)+1]=128+g2;
				data[(i*3)+2]=128+b2;
			}
			if (j%8==4) {
				data[(i*3)]=128+r2;	/* Bright Green */
				data[(i*3)+1]=128+g2;
				data[(i*3)+2]=128+b2;
			}

			if (j%8==5) {
				data[(i*3)]=128+r2/4;	/* Less Bright Green */
				data[(i*3)+1]=128+g2/4;
				data[(i*3)+2]=128+b2/4;
			}

			if (j%8==6) {
				data[(i*3)]=128+r1/4;	/* Less Bright Red */
				data[(i*3)+1]=128+g1/4;
				data[(i*3)+2]=128+b1/4;
			}

			if (j%8==7) {
				data[(i*3)]=128+r1;	/* Bright Red */
				data[(i*3)+1]=128+g1;
				data[(i*3)+2]=128+b1;
			}

		}

		lpd8806_write(spi_fd,data);

		usleep(200000);

	}

	return 0;
}
