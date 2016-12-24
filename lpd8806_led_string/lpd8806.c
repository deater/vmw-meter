#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <linux/spi/spidev.h>

#include "spi_lib.h"

#include "lpd8806.h"

//#include "colors.h"


static unsigned char zeros[128];

int lpd8806_init(void) {

	int spi_fd,i;
	int result;

	spi_fd=spi_open("/dev/spidev0.0", SPI_MODE_0, 100000, 8);
	if (spi_fd<0) {
		return spi_fd;
	}

	/* Send a byte acting as a start bit */

	for(i=0;i<128;i++) zeros[i]=0;

	for(i=0;i<128;i++) {
		result=write(spi_fd,&zeros[i],1);
		if (result<1) {
			printf("error!\n");
			exit(-1);
		}
	}
	return spi_fd;
}

int lpd8806_write(int spi_fd, unsigned char *data) {

	int i,result;

	for(i=0;i<128;i++) {
		result=write(spi_fd,&data[i],1);
		if (result<1) {
			printf("error!\n");
			return result;
		}
	}

	for(i=0;i<128;i++) {
		result=write(spi_fd,&zeros[i],1);
		if (result<1) {
			printf("error!\n");
			return result;
		}
	}
	return 0;
}

int lpd8806_close(int spi_fd) {

	int result;

	result=spi_close(spi_fd);

	return result;
}
