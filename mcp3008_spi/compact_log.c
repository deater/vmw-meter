#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#include <linux/spi/spidev.h>

#include <sys/time.h>

#include "spi_lib.h"


/* So the way the Pi cluster works */
/* Each 5V power rail has a 0.1 Ohm resistor across it */
/* A differential amplifier (MCP6004) amplifies the voltage drop by 20 */
/* This is fed into the even channels of the MCP3008 */
/* 5V is fed into the odd channels */

/* WHy?  possibly I meant to make it possible to have an external */
/* Reference 5v, but in the end all the 5V lines are tied together */

int main(int argc, char **argv) {

	int spi_fd,out_fd,j;
	int value;
	unsigned char data[3];
	int result;
	double rate=4; /* Hz */
	int period;

	char hostname[BUFSIZ],filename[BUFSIZ];
	char cbuffer[100];
	short sbuffer[6];

	struct timeval last_time,current_time;

	gethostname(hostname,BUFSIZ);

	printf("hostname=%s\n",hostname);

	gettimeofday(&last_time,NULL);

	spi_fd=spi_open("/dev/spidev0.0", SPI_MODE_0, 1000000, 8);
	if (spi_fd<0) {
		exit(-1);
	}

	sprintf(filename,"%s.%lld.raw",hostname,(long long)last_time.tv_sec);

	out_fd=open(filename,O_WRONLY|O_CREAT,S_IRUSR|S_IWUSR);
	if (out_fd<0) {
		printf("error opening, %s\n",strerror(errno));
		exit(-1);
	}

	cbuffer[0]='V';
	cbuffer[1]='M';
	cbuffer[2]='W';
	cbuffer[3]='*';
	write(out_fd,cbuffer,4);

	write(out_fd,&(last_time.tv_sec),sizeof(last_time.tv_sec));
	write(out_fd,&(last_time.tv_usec),sizeof(last_time.tv_usec));

	period=(int)(1000000.0*(1.0/rate));


	printf("Reading ADC at %dHz (%dus)\n",
		(int)rate,period);

	while(1) {

		int usec_diff;
		int s_diff;
		int total_diff;
		gettimeofday(&current_time,NULL);
		usec_diff=current_time.tv_usec-last_time.tv_usec;
		s_diff=current_time.tv_sec-last_time.tv_sec;
		total_diff=(s_diff*1000000)+usec_diff;
//		printf("diff=%d\n",total_diff);
		memcpy(&last_time,&current_time,sizeof(current_time));

		sbuffer[0]=total_diff>>16;
		sbuffer[1]=total_diff&0xffff;

		for(j=0;j<8;j+=2) {

			/* Send a byte acting as a start bit */
			data[0] = 1;

			/* Ask for regular input output */
			/* High/Low */
//			data[1]=0x20;
			data[1] = ((j) & 0x7) << 4;
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
//			printf("\t%d",value);
			sbuffer[2+(j/2)]=value;
		}
		write(out_fd,sbuffer,6*sizeof(short));
		usleep(period);

	}

	spi_close(spi_fd);
	close(out_fd);
	return 0;
}
