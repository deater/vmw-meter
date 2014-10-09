#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <linux/spi/spidev.h>

#include "mcp3008_spi.h"

#define SPEED 1000000

int main(int argc, char **argv) {

	int spi_fd,j;
	int value[2];
	unsigned char data[3];
	int result;
	double voltage,current,power,deltav,ref;

	spi_fd=spi_open("/dev/spidev0.0", SPI_MODE_0, SPEED, 8);
	if (spi_fd<0) {
		exit(-1);
	}

	while(1) {
		printf("Result:");

		for(j=0;j<2;j++) {

			/* Send a byte acting as a start bit */
			data[0] = 1;

			/* Ask for differential output */
			/* High/Low */
			data[1] = ((j & 0x7) << 4);
			data[1] |=0x80;

			/* Don't care, need 3 bytes before response */
			data[2] = 0;

			result=spi_writeread(spi_fd, data,
						sizeof(data), SPEED, 8 );
			if (result<0) {
				exit(-1);
			}

			/* 3-byte result */
			/* XXXXXXXX */
			/* XXXXX098 */
			/* 76543210 */
			value[j] = ((data[1]&0x3) << 8) | (data[2] & 0xff);
			printf("\t%d",value[j]);
		}
		ref=4.90;
		voltage=((double)value[0])/1024.0;
		voltage*=ref;
		deltav=voltage/20.0;
		current=deltav/0.1;
		power=ref*current;

		printf(" Voltage=%.3lfV DeltaV=%.3lfV Current=%.3lfA Power=%.2lfW\n",voltage,deltav,current,power);
		printf("\n");
		sleep(1);

	}

	spi_close(spi_fd);

	return 0;
}
