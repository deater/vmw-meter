#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <math.h>

#include "colors.h"



#include <linux/spi/spidev.h>

#include "spi_lib.h"

#define MAX_BRIGHTNESS 64

int main(int argc, char **argv) {

	int spi_fd,i;
	unsigned char zeros[128],data[128];
	int result;

	int r,g,b;

        /* color left */
        if (argc>1) {
                get_color(argv[1],&r,&g,&b);
        }
        else {
                r=63;
                g=0;
                b=63;
        }


	/* Open the SPI device */

	spi_fd=spi_open("/dev/spidev0.0", SPI_MODE_0, 100000, 8);
	if (spi_fd<0) {
		exit(-1);
	}

	/* Zero out our framebuffer */
	for(i=0;i<128;i++) zeros[i]=0;

	/* Set data to all black */
	for(i=0;i<128;i++) data[i]=128;

	/* Set display to black */
	for(i=0;i<128;i++) {
		result=write(spi_fd,&zeros[i],1);
		if (result<1) {
			printf("error!\n");
			exit(-1);
		}
	}


#define PI 3.14159265358979323846264
#define QUANTUM (PI/8.0)

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

		for(i=0;i<96;i++) {
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
