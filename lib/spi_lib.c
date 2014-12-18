#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#include <sys/ioctl.h>
#include <linux/spi/spidev.h>

#include "spi_lib.h"

int spi_open(char *dev_name, int mode, int freq, int bitsperword) {

	int spi_fd;

	int result=-1;

	spi_fd=open(dev_name, O_RDWR);
	if (spi_fd < 0) {
		fprintf(stderr,"Could not open SPI device %s : %s\n",
			dev_name,strerror(errno));
		return -1;
	}

	result = ioctl(spi_fd, SPI_IOC_WR_MODE, &mode);
	if (result < 0) {
		fprintf(stderr,"Could not set SPI Write mode: %s\n",
			strerror(errno));
		return -1;
	}

	result = ioctl(spi_fd, SPI_IOC_RD_MODE, &mode);
	if (result < 0) {
		fprintf(stderr,"Could not set SPI Read mode: %s\n",
			strerror(errno));
		return -1;
	}

	result = ioctl(spi_fd, SPI_IOC_WR_BITS_PER_WORD, &bitsperword);
	if (result < 0) {
		fprintf(stderr,"Could not set SPI WR bitsPerWord: %s\n",
			strerror(errno));
		return -1;
	}

	result = ioctl(spi_fd, SPI_IOC_RD_BITS_PER_WORD, &bitsperword);
	if (result < 0) {
		fprintf(stderr,"Could not set SPI RD bitsPerWord: %s\n",
			strerror(errno));
		return -1;
	}

	result = ioctl(spi_fd, SPI_IOC_WR_MAX_SPEED_HZ, &freq);
	if (result < 0) {
		fprintf(stderr,"Could not set SPI WR frequency: %s\n",
			strerror(errno));
		return -1;
	}

	result = ioctl(spi_fd, SPI_IOC_RD_MAX_SPEED_HZ, &freq);
	if (result < 0) {
		fprintf(stderr,"Could not set SPI RD frequency: %s\n",
			strerror(errno));
		return -1;
	}

	return spi_fd;
}

int spi_close(int spi_fd) {

	int result;

	result = close(spi_fd);

	if (result < 0) {
		fprintf(stderr,"Could not close SPI device: %s\n",
			strerror(errno));
		return -1;
	}
	return result;
}

int spi_writeread(int spi_fd, unsigned char *data, int length,
		int freq, int bitsperword ) {

	struct spi_ioc_transfer spi[length];
	int i = 0;
	int result = -1;

	/* one spi transfer for each byte */
	for(i=0; i<length; i++) {
		spi[i].tx_buf	= (unsigned long)(data + i);
		spi[i].rx_buf	= (unsigned long)(data + i);
		spi[i].len		= 1;	/* 1 byte */
		spi[i].delay_usecs	= 0 ;
		spi[i].speed_hz		= freq ;
		spi[i].bits_per_word	= bitsperword ;
		spi[i].cs_change	= 0;
	}

	result = ioctl(spi_fd, SPI_IOC_MESSAGE(length), &spi) ;

	if (result < 0){
		fprintf(stderr,"Error sending SPI data: %s\n",strerror(errno));
		return -1;
	}

	return result;
}

