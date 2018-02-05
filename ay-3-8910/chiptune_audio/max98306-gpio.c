#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include "max98306.h"

#define SHUTDOWN_GPIO	21
#define HEADPHONE_GPIO	20
#define GAIN_GPIO	12
#define GAIN_PRIME_GPIO	13


int max98306_init(void) {

	int fd;
	char buffer[BUFSIZ];

	/* Initialize the GPIOs */

	/************/
	/* SHUTDOWN */
	/************/

	sprintf(buffer,"%d",SHUTDOWN_GPIO);
	fd=open("/sys/class/gpio/export",O_WRONLY);
	if (fd<0) {
		printf("Error opening %s\n",strerror(errno));
	}
	else {
		write(fd,buffer,strlen(buffer));
		close(fd);
	}

	/* Set direction to output */
	sprintf(buffer,"/sys/class/gpio/gpio%d/direction",SHUTDOWN_GPIO);
	fd=open(buffer,O_WRONLY);
	if (fd<0) {
		printf("Error direction %s\n",strerror(errno));
	}
	else {
		write(fd,"out",3);
		close(fd);
	}


	/*************************/
	/* HEADPHONE JACK DETECT */
	/*************************/

	sprintf(buffer,"%d",HEADPHONE_GPIO);
	fd=open("/sys/class/gpio/export",O_WRONLY);
	if (fd<0) {
		printf("Error opening %s\n",strerror(errno));
	}
	else {
		write(fd,buffer,strlen(buffer));
		close(fd);
	}

	/* Set direction to input */
	sprintf(buffer,"/sys/class/gpio/gpio%d/direction",HEADPHONE_GPIO);
	fd=open(buffer,O_WRONLY);
	if (fd<0) {
		printf("Error direction %s\n",strerror(errno));
	}
	else {
		write(fd,"in",2);
		close(fd);
	}

	/********/
	/* GAIN */
	/********/

	sprintf(buffer,"%d",GAIN_GPIO);
	fd=open("/sys/class/gpio/export",O_WRONLY);
	if (fd<0) {
		printf("Error opening %s\n",strerror(errno));
	}
	else {
		write(fd,buffer,strlen(buffer));
		close(fd);
	}

	/**************/
	/* GAIN_PRIME */
	/**************/

	sprintf(buffer,"%d",GAIN_PRIME_GPIO);
	fd=open("/sys/class/gpio/export",O_WRONLY);
	if (fd<0) {
		printf("Error opening %s\n",strerror(errno));
	}
	else {
		write(fd,buffer,strlen(buffer));
		close(fd);
	}

	/* Set volume to low as default */
	max98306_set_volume(1);

	return 0;
}

int max98306_check_headphone(void) {

	int fd;
	char buffer[BUFSIZ];
	int value=0;

	/* Read Headphone GPIO */
	sprintf(buffer,"/sys/class/gpio/gpio%d/value",HEADPHONE_GPIO);
	fd=open(buffer,O_RDONLY);
	if (fd<0) {
		printf("Error opening value %s\n",strerror(errno));
		return -1;
	}
	read(fd,buffer,1);
	close(fd);
	value=buffer[0];
	return (value-'0');

}

#define GAIN_LOW	0
#define GAIN_HIGH	1
#define GAIN_FLOAT	2

int max98306_set_volume(int value) {

	int volume;
	int gain_setting,gain_prime_setting;
	char buffer[BUFSIZ];
	int fd;

	/*
				GAIN		GAIN_PRIME
		0 = shutdown
		1 =  6dB	FLOATING	FLOATING
		2 =  9dB	FLOATING	HIGH
		3 = 12dB	HIGH		FLOATING
		4 = 15dB	FLOATING	LOW
		5 = 18dB	LOW		FLOATING
	*/

	if (value<0) volume=0;
	else if (value>5) volume=5;
	else volume=value;

	if (volume==0) {
		max98306_disable();
		return 0;
	}

	/* in case we had been disabled */
	max98306_enable();

	switch(volume) {
		case 1:	/*  6dB	FLOATING	FLOATING	*/
			gain_setting=GAIN_FLOAT;
			gain_prime_setting=GAIN_FLOAT;
			break;
		case 2: /*  9dB	FLOATING	HIGH		*/
			gain_setting=GAIN_FLOAT;
			gain_prime_setting=GAIN_HIGH;
			break;
		case 3: /* 12dB	HIGH		FLOATING	*/
			gain_setting=GAIN_HIGH;
			gain_prime_setting=GAIN_FLOAT;
			break;
		case 4: /* 15dB	FLOATING	LOW		*/
			gain_setting=GAIN_FLOAT;
			gain_prime_setting=GAIN_LOW;
			break;
		case 5: /* 18dB	LOW		FLOATING	*/
			gain_setting=GAIN_LOW;
			gain_prime_setting=GAIN_FLOAT;
			break;
		default:
			fprintf(stderr,"Unknown volume %d\n",volume);
			return -1;
	}

	/* Set GAIN value */
	sprintf(buffer,"/sys/class/gpio/gpio%d/direction",GAIN_GPIO);
	fd=open(buffer,O_WRONLY);
	if (fd<0) {
		printf("Error direction %s\n",strerror(errno));
	}

	if (gain_setting==GAIN_FLOAT) {
		/* Set float by making an INPUT */
		write(fd,"in",2);
		close(fd);
	}
	else {
		write(fd,"out",3);
		close(fd);

		/* Write Value */
		sprintf(buffer,"/sys/class/gpio/gpio%d/value",GAIN_GPIO);
		fd=open(buffer,O_WRONLY);
		if (fd<0) {
			printf("Error opening value %s\n",strerror(errno));
			return -1;
		}
		buffer[0]=gain_setting+'0';
		write(fd,buffer,1);
		close(fd);
	}


	/* Set GAIN_PRIME value */
	sprintf(buffer,"/sys/class/gpio/gpio%d/direction",GAIN_PRIME_GPIO);
	fd=open(buffer,O_WRONLY);
	if (fd<0) {
		printf("Error direction %s\n",strerror(errno));
	}

	if (gain_prime_setting==GAIN_FLOAT) {
		/* Set float by making an INPUT */
		write(fd,"in",2);
		close(fd);
	}
	else {
		write(fd,"out",3);
		close(fd);

		/* Write Value */
		sprintf(buffer,"/sys/class/gpio/gpio%d/value",GAIN_PRIME_GPIO);
		fd=open(buffer,O_WRONLY);
		if (fd<0) {
			printf("Error opening value %s\n",strerror(errno));
			return -1;
		}
		buffer[0]=gain_prime_setting+'0';
		write(fd,buffer,1);
		close(fd);
	}

	return 0;
}


int max98306_disable(void) {

	int fd;
	char buffer[BUFSIZ];

	/* Push !SHUTDOWN low */
        sprintf(buffer,"/sys/class/gpio/gpio%d/value",SHUTDOWN_GPIO);
        fd=open(buffer,O_WRONLY);
        if (fd<0) {
                printf("Error opening value %s\n",strerror(errno));
                return -1;
        }
	write(fd,"0",1);
	close(fd);

	return 0;
}

int max98306_enable(void) {

	int fd;
	char buffer[BUFSIZ];

	/* Push !SHUTDOWN high */
        sprintf(buffer,"/sys/class/gpio/gpio%d/value",SHUTDOWN_GPIO);
        fd=open(buffer,O_WRONLY);
        if (fd<0) {
                printf("Error opening value %s\n",strerror(errno));
                return -1;
        }
	write(fd,"1",1);
	close(fd);

	return 0;
}


int max98306_free(void) {

	max98306_disable();

	return 0;
}
