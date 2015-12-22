#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <linux/spi/spidev.h>

#include <math.h>

#include "spi_lib.h"

#include "colors.h"


int main(int argc, char **argv) {

	int spi_fd,i;
	unsigned char zeros[128],data[128];
	int result;

	int lr,lg,lb,rr,rg,rb;

	/* color left */
	if (argc>1) {
                get_color(argv[1],&lr,&lg,&lb);
        }
        else {
                lr=63;
                lg=0;
                lb=0;
        }


	/* color right */
	if (argc>2) {
                get_color(argv[2],&rr,&rg,&rb);
        }
        else {
                rr=0;
                rg=0;
                rb=0;
        }


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

	int bar=0;

	unsigned char ch;

	while(1) {
		scanf("%c",&ch);

		printf("Pressed %d\n",ch);

		if (ch=='i') {
			bar++;
			if (bar>32) bar=32;
		}
		if (ch=='m') {
			bar--;
			if (bar<0) bar=0;
		}

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
	}

	spi_close(spi_fd);

	return 0;
}
