#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <linux/spi/spidev.h>

#include "mcp3008_spi.h"

int main(int argc, char **argv) {

	int spi_fd,j;
	int value;
	unsigned char data[3];
	int result;
	double voltage,current,power;

	spi_fd=spi_open("/dev/spidev0.0", SPI_MODE_0, 1000000, 8);
	if (spi_fd<0) {
		exit(-1);
	}

	while(1) {
		printf("Result:");

		for(j=0;j<4;j++) {

			/* Send a byte acting as a start bit */
			data[0] = 1;

			/* Ask for differential output */
			/* High/Low */
			data[1] = ((j & 0x7) << 5);

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
		voltage=((double)value)/1024.0;
		voltage*=5.0;
		current=voltage/0.1;
		power=5.0*current;

		printf("Voltage=%.3lfV Current=%.3lfA Power=%.2lfW\n",voltage,current,power);
		printf("\n");
		sleep(1);

	}

	spi_close(spi_fd);

	return 0;
}
