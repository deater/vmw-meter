/* Code to read the i2c mpl115a2 Pressure sensor */

/* temperature known not to be entirely accurate */

/* Based loosely on the adafruit code */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>

#include <math.h>

#include <sys/time.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

#include "i2c_lib.h"

#include "mpl115a2.h"

static double c_to_f(double c) {

	return ((9.0/5.0)*c)+32;

}


int main(int argc, char **argv) {

	signed int result;

	float pressure,temp,pressureComp;

	int16_t a0coeff;
	int16_t b1coeff;
	int16_t b2coeff;
	int16_t c12coeff;

	int16_t pressure_raw,temp_raw;

	float mpl115a2_a0;
	float mpl115a2_b1;
	float mpl115a2_c12;
	float mpl115a2_b2;

	int i2c_fd;
	unsigned char buffer[16];

	struct timeval start_time; // now_time;

	gettimeofday(&start_time, NULL);

	i2c_fd=init_i2c("/dev/i2c-1");
	if (i2c_fd < 0) {
		fprintf(stderr,"Error opening device!\n");
		return -1;
	}

	/* Init MPL115A2 */
	if (ioctl(i2c_fd, I2C_SLAVE, MPL115A2_ADDR) < 0) {
		fprintf(stderr,"Error setting i2c address %x\n",
			MPL115A2_ADDR);
		return -1;
	}

        /* Get the coefficients */

	buffer[0] = MPL115A2_REGISTER_A0_COEFF_MSB;
	if ( (write(i2c_fd, &buffer, 1)) !=1) {
		fprintf(stderr,"Error getting coefficients!\n");
		return -1;
	}

        if ( (read(i2c_fd, &buffer, 8)) !=8) {
       	        fprintf(stderr,"Error reading coefficients!\n");
               	return -1;
       	}

	a0coeff = (((uint16_t)buffer[0] << 8) | buffer[1]);
	b1coeff = (((uint16_t)buffer[2] << 8) | buffer[3]);
	b2coeff = (((uint16_t)buffer[4] << 8) | buffer[5]);
	c12coeff = (((uint16_t)buffer[6] << 8) | buffer[7]) >> 2;

	mpl115a2_a0 = (float)a0coeff / 8;
	mpl115a2_b1 = (float)b1coeff / 8192;
	mpl115a2_b2 = (float)b2coeff / 16384;
	mpl115a2_c12 = (float)c12coeff;
	mpl115a2_c12 /= 4194304.0;

	printf("Note, the temperature value isn't that great\n"
		"freescale says it is uncalibrated and not to rely on it\n");

	while(1) {
		buffer[0]=MPL115A2_REGISTER_STARTCONVERSION;
		buffer[1]=0;
		if ( (write(i2c_fd, &buffer, 2)) !=2) {
			fprintf(stderr,"Error starting conversion!\n");
			return -1;
		}


		/* wait at least 3ms */
		usleep(5000);

		buffer[0] = MPL115A2_REGISTER_PRESSURE_MSB;
		if ( (write(i2c_fd, &buffer, 1)) !=1) {
			fprintf(stderr,"Error setting pressure!\n");
			return -1;
		}

	        if ( (read(i2c_fd, &buffer, 4)) !=4) {
       		        fprintf(stderr,"Error reading pressure!\n");
               		return -1;
       		}

		pressure_raw = (((uint16_t)buffer[0] << 8) | buffer[1]) >> 6;
		temp_raw = (((uint16_t)buffer[2] << 8) | buffer[3]) >> 6;

		// See datasheet p.6 for evaluation sequence
		pressureComp = mpl115a2_a0 +
			(mpl115a2_b1 + mpl115a2_c12 * temp_raw) *
			pressure_raw + mpl115a2_b2 * temp_raw;


		pressure = ((65.0F / 1023.0F) * pressureComp) + 50.0F; // kPa
		temp = ((float)temp_raw - 498.0F) / -5.35F + 25.0F;    // C

		printf("Pressure: %.2lf kPa, Temp: %.2lf C (%.2lf F)\n",
			pressure,temp,c_to_f(temp));

		sleep(1);
	}

	return result;
}

