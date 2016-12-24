#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <math.h>


//#include <linux/spi/spidev.h>
//#include "spi_lib.h"

#include "lpd8806.h"

#include "colors.h"

#define PI 3.1415926535897932384


int bargraph(int spi_fd,
		char *left_color,
		char *right_color) {

	int i;
	unsigned char data[128];
	int lr,lg,lb,rr,rg,rb;
	int bar=0;
	double x=0;
	double f=(PI/16);

	printf("Running bargraph %s %s\n",left_color,right_color);

	/* color left */
	if (left_color) {
                get_color(left_color,&lr,&lg,&lb);
        }
        else {
                lr=63;
                lg=0;
                lb=0;
        }


	/* color right */
	if (right_color) {
                get_color(right_color,&rr,&rg,&rb);
        }
        else {
                rr=0;
                rg=0;
                rb=0;
        }

	for(i=0;i<128;i++) data[i]=0;

	while(1) {

		x=x+f;
		bar=(int)((sin(x)+1.0)*16.0);

		for(i=0;i<32;i++) {
			if (i<bar) {
				data[(i*3)+0]=128+lg;
				data[(i*3)+1]=128+lr;
				data[(i*3)+2]=128+lb;
			} else {
				data[(i*3)+0]=128+rg;
				data[(i*3)+1]=128+rr;
				data[(i*3)+2]=128+rb;
			}
		}

		lpd8806_write(spi_fd,data);

		usleep(10000);

	}

	return 0;
}
