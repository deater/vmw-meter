/* Code to access TCS34725 color sensor */

/* Roughly based on code found in the adafruit arduino library */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>

#include <string.h>

#include <sys/time.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

#include "i2c_lib.h"

#include "tcs34725.h"

static int debug=1;
static int TCS34725_integration_time = TCS34725_INTEGRATIONTIME_2_4MS;
static int TCS34725_gain = TCS34725_GAIN_1X;


/* little endian? */
static int get_raw_data(int i2c_fd,
		uint16_t *r, uint16_t *g, uint16_t *b, uint16_t *c) {

	uint8_t buffer[16];

	/* read C */
	buffer[0] = TCS34725_CDATAL | TCS34725_COMMAND_BIT;
	if ( (write(i2c_fd, &buffer, 1)) !=1) {
		fprintf(stderr,"Error setting C!\n");
		return -1;
	}
	if ( (read(i2c_fd, &buffer, 2)) !=2) {
		fprintf(stderr,"Error reading C!\n");
		return -1;
	}
	*c = ((buffer[1]<<8)|buffer[0]);
	if (debug) printf("\tRead C=%x\n",*c);

	/* read R */
	buffer[0] = TCS34725_RDATAL | TCS34725_COMMAND_BIT;
	if ( (write(i2c_fd, &buffer, 1)) !=1) {
		fprintf(stderr,"Error setting R!\n");
		return -1;
	}
	if ( (read(i2c_fd, &buffer, 2)) !=2) {
		fprintf(stderr,"Error reading R!\n");
		return -1;
	}
	*r = ((buffer[1]<<8)|buffer[0]);
	if (debug) printf("\tRead R=%x\n",*r);

	/* read G */
	buffer[0] = TCS34725_GDATAL | TCS34725_COMMAND_BIT;
	if ( (write(i2c_fd, &buffer, 1)) !=1) {
		fprintf(stderr,"Error setting G!\n");
		return -1;
	}
	if ( (read(i2c_fd, &buffer, 2)) !=2) {
		fprintf(stderr,"Error reading G!\n");
		return -1;
	}
	*g = ((buffer[1]<<8)|buffer[0]);
	if (debug) printf("\tRead G=%x\n",*g);

	/* read B */
	buffer[0] = TCS34725_BDATAL | TCS34725_COMMAND_BIT;
	if ( (write(i2c_fd, &buffer, 1)) !=1) {
		fprintf(stderr,"Error setting B!\n");
		return -1;
	}
	if ( (read(i2c_fd, &buffer, 2)) !=2) {
		fprintf(stderr,"Error reading B!\n");
		return -1;
	}
	*b = ((buffer[1]<<8)|buffer[0]);
	if (debug) printf("\tRead B=%x\n",*b);

	/* Set a delay for the integration time in ms */
	/* 12/5 = 2.4, add 1 to account for integer truncation */
	usleep(1000*(256 - TCS34725_integration_time) * 12 / 5 + 1);

	return 0;
}



static void get_RGB(int i2c_fd, float *r, float *g, float *b) {

	uint16_t red=0, green=0, blue=0, clear=0;
	uint32_t sum;

	get_raw_data(i2c_fd, &red, &green, &blue, &clear);

	sum=clear;

	/* Avoid divide by zero errors; if clear return black */
	if (clear == 0) {
		*r = *g = *b = 0;
		return;
	}

	*r = (float)red / sum * 255.0;
	*g = (float)green / sum * 255.0;
	*b = (float)blue / sum * 255.0;
}


static int set_integration_time(int i2c_fd, uint8_t it) {

	uint8_t buffer[16];

	/* Update the timing register */
	buffer[0] = TCS34725_ATIME | TCS34725_COMMAND_BIT;
	buffer[1] = it;
	if ( (write(i2c_fd, &buffer, 2)) !=2) {
		fprintf(stderr,"Error setting ID!\n");
		return -1;
	}

	/* Update value placeholders */
	TCS34725_integration_time = it;

	return 0;
}

static int set_gain(int i2c_fd, uint8_t gain) {

	uint8_t buffer[16];

	/* Update the gain register */
	buffer[0] = TCS34725_CONTROL | TCS34725_COMMAND_BIT;
	buffer[1] = gain;
	if ( (write(i2c_fd, &buffer, 2)) !=2) {
		fprintf(stderr,"Error setting ID!\n");
		return -1;
	}

	/* Update value placeholders */
	TCS34725_gain = gain;

	return 0;
}


static int enable_tcs34725(int i2c_fd) {

	uint8_t buffer[16];

	buffer[0] = TCS34725_ENABLE | TCS34725_COMMAND_BIT;
	buffer[1] = TCS34725_ENABLE_PON;
	if ( (write(i2c_fd, &buffer, 2)) !=2) {
		fprintf(stderr,"Error enabling!\n");
		return -1;
	}

	usleep(3000);


	buffer[0] = TCS34725_ENABLE | TCS34725_COMMAND_BIT;
	buffer[1] = TCS34725_ENABLE_PON | TCS34725_ENABLE_AEN;
	if ( (write(i2c_fd, &buffer, 2)) !=2) {
		fprintf(stderr,"Error enabling!\n");
		return -1;
	}

	/* Set a delay for the integration time.
	This is only necessary in the case where enabling and then
	immediately trying to read values back. This is because setting
	AEN triggers an automatic integration, so if a read RGBC is
	performed too quickly, the data is not yet valid and all 0's are
	returned */
	/* 12/5 = 2.4, add 1 to account for integer truncation */
	usleep(1000*((256 - TCS34725_integration_time) * 12 / 5 + 1));

	return 0;
}

/* disable device, put in low power sleep mode */
static int disable_tcs34725(int i2c_fd) {

	uint8_t buffer[16];

	/* Turn the device off to save power */
	uint8_t reg = 0;

	buffer[0] = TCS34725_ENABLE | TCS34725_COMMAND_BIT;
	if ( (write(i2c_fd, &buffer, 1)) !=1) {
		fprintf(stderr,"Error disabling!\n");
		return -1;
	}
	if ( (read(i2c_fd, &reg, 1)) !=1) {
		fprintf(stderr,"Error reading reg!\n");
		return -1;
	}

	buffer[0] = TCS34725_ENABLE | TCS34725_COMMAND_BIT;
	buffer[1] = reg & ~(TCS34725_ENABLE_PON | TCS34725_ENABLE_AEN);
	if ( (write(i2c_fd, &buffer, 2)) !=2) {
		fprintf(stderr,"Error disabling!\n");
		return -1;
	}

	return 0;
}


static int init_tcs34725(int i2c_fd) {

	uint8_t buffer[16],id;

	/* Make sure we're actually connected */

	buffer[0] = TCS34725_ID | TCS34725_COMMAND_BIT;
	if ( (write(i2c_fd, &buffer, 1)) !=1) {
		fprintf(stderr,"Error setting ID!\n");
		return -1;
	}
	if ( (read(i2c_fd, &buffer, 1)) !=1) {
		fprintf(stderr,"Error reading ID!\n");
		return -1;
	}
	id = buffer[0];
	if (debug) printf("\tRead id=%x\n",id);

	if ((id != 0x4d) && (id != 0x44) && (id != 0x10)) {
		return -1;
	}

	/* Set default integration time and gain */
	set_integration_time(i2c_fd,TCS34725_integration_time);
	set_gain(i2c_fd,TCS34725_gain);

	/* Note: by default, the device is in power down mode on bootup */
	enable_tcs34725(i2c_fd);

	return 0;
}

int main(int argc, char **argv) {

	signed int result;

	int i2c_fd;
	float r,g,b;

	i2c_fd=init_i2c("/dev/i2c-1");
	if (i2c_fd < 0) {
		fprintf(stderr,"Error opening device!\n");
		return -1;
	}

	/* Init TCS34725 */
	if (ioctl(i2c_fd, I2C_SLAVE, TCS34725_I2C_ADDRESS) < 0) {
		fprintf(stderr,"Error setting i2c address %x\n",
			TCS34725_I2C_ADDRESS);
		return -1;
	}


	if (init_tcs34725(i2c_fd)) {
		fprintf(stderr,"Error initializing TCS34725\n");

		return -1;
	}

	while(1) {

		get_RGB(i2c_fd,&r,&g,&b);

		printf("r=%.2f, g=%.2f, b=%.2f\n",r,g,b);

		sleep(1);
	}

	disable_tcs34725(i2c_fd);

	return result;
}
