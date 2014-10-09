#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <linux/spi/spidev.h>

#include "mcp3008_spi.h"

int main(int argc, char **argv) {

	int spi_fd,i,j;
	int value;
	unsigned char data[3];
	int result;

	spi_fd=spi_open("/dev/spidev0.0", SPI_MODE_0, 1000000, 8);
	if (spi_fd<0) {
		exit(-1);
	}

	printf("\tCh0\tCh1\tCh2\tCh3\n");
	for(i=0;i<30;i++) {

		printf("Result:");

		for(j=0;j<4;j++) {

			/* Send a byte acting as a start bit */
			data[0] = 1;

			/* Ask for differential output */
			/* High/Low */
			data[1] = ((j & 0x7) << 5);
			data[1]|=0x80;

			/* Don't care, need 3 bytes before response */
			data[2] = 0;

			result=spi_writeread(spi_fd, data,
						sizeof(data), 1000000, 8 );
			if (result<0) {
				exit(-1);
			}

			/* 3-byte result */
			/* XXXXXXXX */
			/* XXXXX098 */
			/* 76543210 */
			value = ((data[1]&0x3) << 8) | (data[2] & 0xff);
			printf("\t%d",value);
		}
		printf("\n");
		sleep(1);

	}

	spi_close(spi_fd);

	return 0;
}
