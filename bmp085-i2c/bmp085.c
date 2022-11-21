/* Code to read the i2c bmp280 Pressure sensor */

/* Based loosely on the adafruit code */


#define BMP085_I2CADDR		0x77	// BMP085 I2C address

#define BMP085_ULTRALOWPOWER	0	// Ultra low power mode
#define BMP085_STANDARD		1	// Standard mode
#define BMP085_HIGHRES		2       // High-res mode
#define BMP085_ULTRAHIGHRES	3	// Ultra high-res mode
#define BMP085_CAL_AC1		0xAA	//	Calibration data (16 bits)
#define BMP085_CAL_AC2		0xAC	//	Calibration data (16 bits)
#define BMP085_CAL_AC3		0xAE	//	Calibration data (16 bits)
#define BMP085_CAL_AC4		0xB0	//	Calibration data (16 bits)
#define BMP085_CAL_AC5		0xB2	//	Calibration data (16 bits)
#define BMP085_CAL_AC6		0xB4	//!< R  Calibration data (16 bits)
#define BMP085_CAL_B1		0xB6	//!< R   Calibration data (16 bits)
#define BMP085_CAL_B2		0xB8	//!< R   Calibration data (16 bits)
#define BMP085_CAL_MB		0xBA	//!< R   Calibration data (16 bits)
#define BMP085_CAL_MC		0xBC	//	Calibration data (16 bits)
#define BMP085_CAL_MD		0xBE	//	Calibration data (16 bits)

#define BMP085_CONTROL		0xF4	// Control register
#define BMP085_TEMPDATA		0xF6	// Temperature data register
#define BMP085_PRESSUREDATA	0xF6	// Pressure data register
#define BMP085_READTEMPCMD	0x2E	// Read temperature control register value
#define BMP085_READPRESSURECMD	0x34	// Read pressure control register value


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>

#include <math.h>

#include <sys/time.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

#include "i2c_lib.h"

static int16_t ac1, ac2, ac3, b1, b2, mb, mc, md;
static uint16_t ac4, ac5, ac6;

static double c_to_f(double c) {

	return ((9.0/5.0)*c)+32;

}

static int32_t compute_B5(int32_t UT) {

	int32_t X1,X2;

	X1 = (UT - (int32_t)ac6) * ((int32_t)ac5) >> 15;
	X2 = ((int32_t)mc << 11) / (X1 + (int32_t)md);

	return X1 + X2;
}


static uint16_t read_raw_temperature(int i2c_fd) {

	uint8_t buffer[17];
	uint16_t temp;

	buffer[0] = BMP085_CONTROL;
	buffer[1] = BMP085_READTEMPCMD;
	if ( (write(i2c_fd, &buffer, 2)) !=2) {
		fprintf(stderr,"Error writing temp reg!\n");
		return -1;
	}

	/* wait 4.5ms */
	usleep(5000);

	buffer[0] = BMP085_TEMPDATA;
	if ( (write(i2c_fd, &buffer, 1)) !=1) {
		fprintf(stderr,"Error setting AC1!\n");
		return -1;
	}
	if ( (read(i2c_fd, &buffer, 2)) !=2) {
		fprintf(stderr,"Error reading AC1!\n");
		return -1;
       	}
	temp = buffer[1] | (buffer[0] << 8);

	return temp;
}

static uint32_t read_raw_pressure(int i2c_fd,int oversampling) {

	static uint32_t raw;
	uint8_t buffer[17];

	buffer[0] = BMP085_CONTROL;
	buffer[1] = BMP085_READPRESSURECMD+(oversampling << 6);
	if ( (write(i2c_fd, &buffer, 2)) !=2) {
		fprintf(stderr,"Error writing temp reg!\n");
		return -1;
	}

	if (oversampling == BMP085_ULTRALOWPOWER) {
		usleep(5000);
	} else if (oversampling == BMP085_STANDARD) {
		usleep(8000);
	}
	else if (oversampling == BMP085_HIGHRES) {
		usleep(14000);
	}
	else {
		usleep(26000);
	}

	buffer[0] = BMP085_PRESSUREDATA;
	if ( (write(i2c_fd, &buffer, 1)) !=1) {
		fprintf(stderr,"Error setting PRESSURE!\n");
		return -1;
	}
	if ( (read(i2c_fd, &buffer, 3)) !=3) {
		fprintf(stderr,"Error reading PRESSURE!\n");
		return -1;
       	}

	raw = (buffer[0]<<16) | (buffer[1] << 8) | (buffer[2]<<8);
	raw >>= (8 - oversampling);

	return raw;
}


int main(int argc, char **argv) {

	signed int result;

	float pressure,temperature;

	int oversampling;

	int32_t ut, up, b3, b5, b6, x1, x2, x3, p;
	uint32_t b4, b7;


	int i2c_fd;
	unsigned char buffer[16];

	struct timeval start_time; // now_time;

	gettimeofday(&start_time, NULL);

	i2c_fd=init_i2c("/dev/i2c-1");
	if (i2c_fd < 0) {
		fprintf(stderr,"Error opening device!\n");
		return -1;
	}



	/* Init BMP085 */
	if (ioctl(i2c_fd, I2C_SLAVE, BMP085_I2CADDR) < 0) {
		fprintf(stderr,"Error setting i2c address %x\n",
			BMP085_I2CADDR);
		return -1;
	}

	buffer[0] = 0xD0; // ?
	if ( (write(i2c_fd, &buffer, 1)) !=1) {
		fprintf(stderr,"Error writing!\n");
		return -1;
	}

        if ( (read(i2c_fd, &buffer, 1)) !=1) {
       	        fprintf(stderr,"Error reading\n");
               	return -1;
       	}

	if (buffer[0]!=0x55) {
		fprintf(stderr,"Error doing something!\n");
		return -1;
	}


        /* Get the calibration data */
	buffer[0] = BMP085_CAL_AC1;
	if ( (write(i2c_fd, &buffer, 1)) !=1) {
		fprintf(stderr,"Error setting AC1!\n");
		return -1;
	}
	if ( (read(i2c_fd, &buffer, 2)) !=2) {
		fprintf(stderr,"Error reading AC1!\n");
		return -1;
       	}
	ac1 = buffer[1] | (buffer[0] << 8);

	buffer[0] = BMP085_CAL_AC2;
	if ( (write(i2c_fd, &buffer, 1)) !=1) {
		fprintf(stderr,"Error setting AC2!\n");
		return -1;
	}
	if ( (read(i2c_fd, &buffer, 2)) !=2) {
		fprintf(stderr,"Error reading AC2!\n");
		return -1;
       	}
	ac2 = buffer[1] | (buffer[0] << 8);

	buffer[0] = BMP085_CAL_AC3;
	if ( (write(i2c_fd, &buffer, 1)) !=1) {
		fprintf(stderr,"Error setting AC3!\n");
		return -1;
	}
	if ( (read(i2c_fd, &buffer, 2)) !=2) {
		fprintf(stderr,"Error reading AC3!\n");
		return -1;
       	}
	ac3 = buffer[1] | (buffer[0] << 8);

	buffer[0] = BMP085_CAL_AC4;
	if ( (write(i2c_fd, &buffer, 1)) !=1) {
		fprintf(stderr,"Error setting AC4!\n");
		return -1;
	}
	if ( (read(i2c_fd, &buffer, 2)) !=2) {
		fprintf(stderr,"Error reading AC4!\n");
		return -1;
       	}
	ac4 = buffer[1] | (buffer[0] << 8);

	buffer[0] = BMP085_CAL_AC5;
	if ( (write(i2c_fd, &buffer, 1)) !=1) {
		fprintf(stderr,"Error setting AC5!\n");
		return -1;
	}
	if ( (read(i2c_fd, &buffer, 2)) !=2) {
		fprintf(stderr,"Error reading AC5!\n");
		return -1;
       	}
	ac5 = buffer[1] | (buffer[0] << 8);

	buffer[0] = BMP085_CAL_AC6;
	if ( (write(i2c_fd, &buffer, 1)) !=1) {
		fprintf(stderr,"Error setting AC6!\n");
		return -1;
	}
	if ( (read(i2c_fd, &buffer, 2)) !=2) {
		fprintf(stderr,"Error reading AC6!\n");
		return -1;
       	}
	ac6 = buffer[1] | (buffer[0] << 8);

	/* cal B */

	buffer[0] = BMP085_CAL_B1;
	if ( (write(i2c_fd, &buffer, 1)) !=1) {
		fprintf(stderr,"Error setting B1!\n");
		return -1;
	}
	if ( (read(i2c_fd, &buffer, 2)) !=2) {
		fprintf(stderr,"Error reading B1!\n");
		return -1;
       	}
	b1 = buffer[1] | (buffer[0] << 8);

	buffer[0] = BMP085_CAL_B2;
	if ( (write(i2c_fd, &buffer, 1)) !=1) {
		fprintf(stderr,"Error setting B2!\n");
		return -1;
	}
	if ( (read(i2c_fd, &buffer, 2)) !=2) {
		fprintf(stderr,"Error reading B2!\n");
		return -1;
       	}
	b2 = buffer[1] | (buffer[0] << 8);


	/* Cal M */

	buffer[0] = BMP085_CAL_MB;
	if ( (write(i2c_fd, &buffer, 1)) !=1) {
		fprintf(stderr,"Error setting MB!\n");
		return -1;
	}
	if ( (read(i2c_fd, &buffer, 2)) !=2) {
		fprintf(stderr,"Error reading MB!\n");
		return -1;
       	}
	mb = buffer[1] | (buffer[0] << 8);

	buffer[0] = BMP085_CAL_MC;
	if ( (write(i2c_fd, &buffer, 1)) !=1) {
		fprintf(stderr,"Error setting MC!\n");
		return -1;
	}
	if ( (read(i2c_fd, &buffer, 2)) !=2) {
		fprintf(stderr,"Error reading MC!\n");
		return -1;
       	}
	mc = buffer[1] | (buffer[0] << 8);

	buffer[0] = BMP085_CAL_MD;
	if ( (write(i2c_fd, &buffer, 1)) !=1) {
		fprintf(stderr,"Error setting MD!\n");
		return -1;
	}
	if ( (read(i2c_fd, &buffer, 2)) !=2) {
		fprintf(stderr,"Error reading M!\n");
		return -1;
       	}
	md = buffer[1] | (buffer[0] << 8);

	while(1) {

		ut=read_raw_temperature(i2c_fd);
		b5 = compute_B5(ut);
		temperature = (b5 + 8) >> 4;
		temperature /= 10;

		ut=read_raw_temperature(i2c_fd);

		printf("Temperature: %.2lfC, %.2lfF\n",
			temperature,c_to_f(temperature));

		oversampling=BMP085_STANDARD;
		up=read_raw_pressure(i2c_fd,oversampling);

		/* do pressure calcs */
		b6 =b5 - 4000;
		x1 = ((int32_t)b2 * ((b6 * b6) >> 12)) >> 11;
		x2 = ((int32_t)ac2 * b6) >> 11;
		x3 = x1 + x2;
		b3 = ((((int32_t)ac1 * 4 + x3) << oversampling) + 2) / 4;

		x1 = ((int32_t)ac3 * b6) >> 13;
		x2 = ((int32_t)b1 * ((b6 * b6) >> 12)) >> 16;
		x3 = ((x1 + x2) + 2) >> 2;
		b4 = ((uint32_t)ac4 * (uint32_t)(x3 + 32768)) >> 15;
		b7 = ((uint32_t)up - b3) * (uint32_t)(50000UL >> oversampling);

		if (b7 < 0x80000000) {
			p = (b7 * 2) / b4;
		} else {
			p = (b7 / b4) * 2;
		}
		x1 = (p >> 8) * (p >> 8);
		x1 = (x1 * 3038) >> 16;
		x2 = (-7357 * p) >> 16;

		p = p + ((x1 + x2 + (int32_t)3791) >> 4);

		pressure = p /1000;

		printf("p=%.2lfkPa\n",pressure);

		sleep(1);
	}

	return result;
}

