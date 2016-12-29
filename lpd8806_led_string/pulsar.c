#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <math.h>

#include "lpd8806.h"
#include "effects.h"
#include "colors.h"

#define PI 3.14159265358979323846264
#define QUANTUM (PI/8.0)

int pulsar(int spi_fd, char *color) {

	int i;
	unsigned char data[128];

	int r,g,b;

        /* color */
        if (color) {
                get_color(color,&r,&g,&b);
        }
        else {
                r=63;
                g=0;
                b=63;
        }

	/* Set data to all black */
	for(i=0;i<128;i++) data[i]=128;

	double f=0.0,x;

	while(1) {

		f=f+QUANTUM;

//		printf("***");
		for(i=0;i<32;i++) {
			x=sin((f+QUANTUM*(double)i))+1.0;
//			x*=32;

//			printf("%d ",(int)x);

			data[(i*3)]=128+g*x;
			data[(i*3)+1]=128+r*x;
			data[(i*3)+2]=128+b*x;
		}
//		printf("\n");

		lpd8806_write(spi_fd,data);

		usleep(100000);

	}

	lpd8806_close(spi_fd);

	return 0;
}
