#include <stdio.h>

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

#include <sys/ioctl.h>
#include <linux/spi/spidev.h>

/* for 74hc595 */
/* want CS active low */
/* pi can only do MSB first(?) */

/* NOTE NOTE NOTE */
/* The problems I was having was most likely due */
/* to 3.3V from Pi not enough to trigger the inputs */
/* So you *do* seem to need a level shifter */
/* And the adafruit bi-directional (i2c) one doesn't seem to work */
/* but the 74HC125 does */


int main(int argc, char **argv) {

	int spi_fd;
	struct spi_ioc_transfer spi;
	int result = -1;
	unsigned char data_out[3];
	unsigned char data_in[3];
	int new_mode;
	int pattern;

	if (argc>1) {
		pattern=strtol(argv[1],NULL,16);
	}
	else {
		pattern=0xa9;
	}

	/* Open SPI device */
	spi_fd=open("/dev/spidev0.0", O_RDWR);
	if (spi_fd < 0) {
		fprintf(stderr,"Could not open SPI device "
			"/dev/sdpidev0.0 : %s\n",
			strerror(errno));
		return -1;
	}

	/* Set SPI Mode_0 */

        int mode=SPI_MODE_0;
//        int mode=SPI_MODE_3;//|SPI_CS_HIGH;
        result = ioctl(spi_fd, SPI_IOC_WR_MODE, &mode);
        if (result < 0) {
                fprintf(stderr,"Could not set SPI Write mode: %s\n",
                        strerror(errno));
                return -1;
        }

        result = ioctl(spi_fd, SPI_IOC_RD_MODE, &new_mode);
        if (result < 0) {
                fprintf(stderr,"Could not read SPI Write mode: %s\n",
                        strerror(errno));
                return -1;
        }

	printf("We are in mode %x\n",new_mode);

/* Set 8 bits per word */

        int bitsperword=8;
        result = ioctl(spi_fd, SPI_IOC_WR_BITS_PER_WORD, &bitsperword);
        if (result < 0) {
                fprintf(stderr,"Could not set SPI WR bitsPerWord: %s\n",
                        strerror(errno));
                return -1;
        }

        /* Set 100 kHz max frequency */

        int freq=100000;
        result = ioctl(spi_fd, SPI_IOC_WR_MAX_SPEED_HZ, &freq);
        if (result < 0) {
                fprintf(stderr,"Could not set SPI WR frequency: %s\n",
                        strerror(errno));
                return -1;
        }

	data_out[0]=pattern;
	data_in[0]=0x0;

	memset(&spi,0,sizeof(struct spi_ioc_transfer));
	spi.tx_buf = (unsigned long)&data_out;
	spi.rx_buf = (unsigned long)&data_in;
	spi.len = 1;    /* 1 byte */
	spi.delay_usecs = 0 ;
	spi.speed_hz = 100000 ;
	spi.bits_per_word = 8 ;
	spi.cs_change = 0;

	result = ioctl(spi_fd, SPI_IOC_MESSAGE(1), &spi) ;

	if (result < 0){
		fprintf(stderr,"Error sending SPI data: %s\n",strerror(errno));
		return -1;
	}

	printf("Wrote out %x read in %x\n",data_out[0],data_in[0]);

	close(spi_fd);

	return 0;
}
