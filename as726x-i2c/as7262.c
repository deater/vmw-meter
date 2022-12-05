/* Code to access AS7262 6-channel visible light sensor */

/* Roughly based on code found in the adafruit arduino library */

/* note: set use_led to 1 to enable white LED when taking samples */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>

#include <string.h>

#include <sys/time.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

#include "i2c_lib.h"

#include "as7262.h"

static int debug=0;


/* as726x data sheet has you do these "virtual" writes */

static int virtual_write(int i2c_fd, uint8_t addr, uint8_t value) {

	uint8_t status;
	uint8_t buffer[16];

	if (debug) printf("Attempting virtual write of %x to addr %x\n",
			value,addr);

	while (1) {
		/* Read I2C status to see if the write buffer ready */

		buffer[0] = AS726X_SLAVE_STATUS_REG;
		if ( (write(i2c_fd, &buffer, 1)) !=1) {
			fprintf(stderr,"Error setting status!\n");
			exit(1);
		}
		if ( (read(i2c_fd, &status, 1)) !=1) {
			fprintf(stderr,"Error reading status!\n");
			exit(1);
		}
		if (debug) printf("\tRead status1 %x\n",status);

		if ((status & AS726X_SLAVE_TX_VALID) == 0) {
			/* No inbound TX pending, OK to write */
			break;
		}
	}


	/* Send virtual register address */
	/* setting bit 7 indicates a pending write */

	buffer[0] = AS726X_SLAVE_WRITE_REG;
	buffer[1] = (addr | 0x80);
	if ( (write(i2c_fd, &buffer, 2)) !=2) {
		fprintf(stderr,"Error setting write reg!\n");
		exit(1);
	}

	if (debug) printf("\tWrote address %x\n",buffer[2]);

	while (1) {
		/* Read I2C status to see if the write buffer ready */

		buffer[0] = AS726X_SLAVE_STATUS_REG;
		if ( (write(i2c_fd, &buffer, 1)) !=1) {
			fprintf(stderr,"Error setting status!\n");
			return -1;
		}
		if ( (read(i2c_fd, &status, 1)) !=1) {
			fprintf(stderr,"Error reading status!\n");
			return -1;
		}
		if (debug) printf("\tRead status2 %x\n",status);

		if ((status & AS726X_SLAVE_TX_VALID) == 0) {
			/* No inbound TX pending, OK to write */
			break;
		}
	}

	/* Send the data to complete the operation. */

	buffer[0] = AS726X_SLAVE_WRITE_REG;
	buffer[1] = value;
	if ( (write(i2c_fd, &buffer, 2)) !=2) {
		fprintf(stderr,"Error writing value!\n");
		return -1;
	}

	if (debug) printf("\tWrote value OK\n");

	return 0;
}

static uint8_t virtual_read(int i2c_fd, uint8_t addr) {

	uint8_t status, d;
	uint8_t buffer[16];

	if (debug) printf("Attempting virtual read of addr %x\n",
			addr);

	while (1) {
		/* Read I1C status to see if read buffer is ready */

		buffer[0] = AS726X_SLAVE_STATUS_REG;
		if ( (write(i2c_fd, &buffer, 1)) !=1) {
			fprintf(stderr,"Error setting status!\n");
			return -1;
		}
		if ( (read(i2c_fd, &status, 1)) !=1) {
			fprintf(stderr,"Error reading status!\n");
			return -1;
		}

		if (debug) printf("\tRead status1 %x\n",status);

		if ((status & AS726X_SLAVE_TX_VALID) == 0) {
			/* No inbound TX pending, OK to write */
			break;
		}
	}

	/* Send virtual register address */
	/* setting bit 7 indicates a pending write */

	buffer[0] = AS726X_SLAVE_WRITE_REG;
	buffer[1] = addr;
	if ( (write(i2c_fd, &buffer, 2)) !=2) {
		fprintf(stderr,"Error setting write reg!\n");
		return -1;
	}

	if (debug) printf("\tWrote register %x\n",buffer[1]);


	while (1) {
		/* Read I2C status to see if the write buffer ready */

		buffer[0] = AS726X_SLAVE_STATUS_REG;
		if ( (write(i2c_fd, &buffer, 1)) !=1) {
			fprintf(stderr,"Error setting status!\n");
			return -1;
		}
		if ( (read(i2c_fd, &status, 1)) !=1) {
			fprintf(stderr,"Error reading status!\n");
			return -1;
		}

		if (debug) printf("\tRead status2 %x\n",status);

		if ((status & AS726X_SLAVE_RX_VALID) != 0) {
			/* Read data ready */
			break;
		}
	}

	/* Read the data to complete the operation. */

	buffer[0] = AS726X_SLAVE_READ_REG;
	if ( (write(i2c_fd, &buffer, 1)) !=1) {
		fprintf(stderr,"Error writing value!\n");
		return -1;
	}

	if ( (read(i2c_fd, &d, 1)) !=1) {
		fprintf(stderr,"Error reading value!\n");
		return -1;
	}

	if (debug) printf("\tRead value %x\n",d);

	return d;
}

static uint8_t control_value=0x0;

static void enable_interrupt(int i2c_fd) {

	control_value|=(1<<6);	// INT = 1;
	virtual_write(i2c_fd, AS726X_CONTROL_SETUP, control_value);
}

static void set_gain(int i2c_fd, uint8_t gain) {

	control_value&=~CONTROL_GAIN_MASK;
	control_value|=gain<<4;
	virtual_write(i2c_fd,AS726X_CONTROL_SETUP, control_value);
}

static void set_conversion_type(int i2c_fd, uint8_t type) {

	control_value&=~CONTROL_BANK_MASK;
	control_value|=type<<2;
	virtual_write(i2c_fd,AS726X_CONTROL_SETUP, control_value);
}

static int data_ready(int i2c_fd) {
	return !!(virtual_read(i2c_fd,AS726X_CONTROL_SETUP) & 0x02);
}

static void start_measurement(int i2c_fd) {
	control_value&=~CONTROL_DATA_RDY_MASK; // DATA_DRY = 0
	virtual_write(i2c_fd,AS726X_CONTROL_SETUP, control_value);

	set_conversion_type(i2c_fd,ONE_SHOT);
}

static uint8_t led_control_value=0x0;

static void set_led_drv_current(int i2c_fd, uint8_t current) {

	led_control_value&=~LED_ICL_DRV_MASK;
	led_control_value|=current<<4; // _led_control.ICL_DRV = current;
	virtual_write(i2c_fd, AS726X_LED_CONTROL, led_control_value);
}

/* Turn off the DRV LED */
static void led_drv_off(int i2c_fd) {
	led_control_value&=~LED_DRV_MASK; //  _led_control.LED_DRV = 0;
	virtual_write(i2c_fd,AS726X_LED_CONTROL, led_control_value);
}

/* Turn on the DRV LED */
static void led_drv_on(int i2c_fd) {
	led_control_value|=LED_DRV_MASK; //  _led_control.LED_DRV = 1;
	virtual_write(i2c_fd,AS726X_LED_CONTROL, led_control_value);
}

static void set_integration_time(int i2c_fd,uint8_t time) {

	uint8_t int_value=time;

	virtual_write(i2c_fd,AS726X_INT_T, int_value);
}


/* read temperature in C */
static uint8_t read_temperature(int i2c_fd) {

	return virtual_read(i2c_fd,AS726X_DEVICE_TEMP);

}

static uint16_t read_channel(int i2c_fd, uint8_t channel) {
	return (virtual_read(i2c_fd,channel) << 8) |
		virtual_read(i2c_fd,channel + 1);
}

static uint16_t readViolet(int i2c_fd) {
	return (read_channel(i2c_fd,AS7262_VIOLET));
}

static uint16_t readBlue(int i2c_fd) {
	return (read_channel(i2c_fd,AS7262_BLUE));
}

static uint16_t readGreen(int i2c_fd) {
	return (read_channel(i2c_fd,AS7262_GREEN));
}

static uint16_t readYellow(int i2c_fd) {
	return (read_channel(i2c_fd,AS7262_YELLOW));
}

static uint16_t readOrange(int i2c_fd) {
	return (read_channel(i2c_fd,AS7262_ORANGE));
}

static uint16_t readRed(int i2c_fd) {
	return (read_channel(i2c_fd,AS7262_RED));
}


static void read_raw_values(int i2c_fd, uint16_t *buf, uint8_t num) {

	int i;

	for (i = 0; i < num; i++) {
		switch (i) {
			case AS726X_VIOLET:
				buf[i] = readViolet(i2c_fd);
				break;
			case AS726X_BLUE:
				buf[i] = readBlue(i2c_fd);
				break;
			case AS726X_GREEN:
				buf[i] = readGreen(i2c_fd);
				break;
			case AS726X_YELLOW:
				buf[i] = readYellow(i2c_fd);
				break;
			case AS726X_ORANGE:
				buf[i] = readOrange(i2c_fd);
				break;
			case AS726X_RED:
				buf[i] = readRed(i2c_fd);
				break;
			default:
				break;
		}
	}
}

static float read_calibrated_value(int i2c_fd, uint8_t channel) {

	uint32_t val = 0;
	float ret;

	val = ((uint32_t)virtual_read(i2c_fd,channel) << 24) |
        	((uint32_t)virtual_read(i2c_fd,channel + 1) << 16) |
        	((uint32_t)virtual_read(i2c_fd,channel + 2) << 8) |
        	(uint32_t)virtual_read(i2c_fd,channel + 3);


	memcpy(&ret, &val, 4);
	return ret;
}

static float readCalibratedViolet(int i2c_fd) {
	return (read_calibrated_value(i2c_fd,AS7262_VIOLET_CALIBRATED));
}

static float readCalibratedBlue(int i2c_fd) {
	return (read_calibrated_value(i2c_fd,AS7262_BLUE_CALIBRATED));
}

static float readCalibratedGreen(int i2c_fd) {
	return (read_calibrated_value(i2c_fd,AS7262_GREEN_CALIBRATED));
}

static float readCalibratedYellow(int i2c_fd) {
	return (read_calibrated_value(i2c_fd,AS7262_YELLOW_CALIBRATED));
}

static float readCalibratedOrange(int i2c_fd) {
	return (read_calibrated_value(i2c_fd,AS7262_ORANGE_CALIBRATED));
}

static float readCalibratedRed(int i2c_fd) {
	return (read_calibrated_value(i2c_fd,AS7262_RED_CALIBRATED));
}



static void read_calibrated_values(int i2c_fd, float *buf, uint8_t num) {

	int i;

	for (i = 0; i < num; i++) {
		switch (i) {
			case AS726X_VIOLET:
				buf[i] = readCalibratedViolet(i2c_fd);
				break;
			case AS726X_BLUE:
				buf[i] = readCalibratedBlue(i2c_fd);
				break;
			case AS726X_GREEN:
				buf[i] = readCalibratedGreen(i2c_fd);
				break;
			case AS726X_YELLOW:
				buf[i] = readCalibratedYellow(i2c_fd);
				break;
			case AS726X_ORANGE:
				buf[i] = readCalibratedOrange(i2c_fd);
				break;
			case AS726X_RED:
				buf[i] = readCalibratedRed(i2c_fd);
				break;
			default:
				break;
		}
	}
}

int main(int argc, char **argv) {

	signed int result;

	int i2c_fd;
	int use_led=0;

	uint8_t version,control_value,temp;
	uint16_t sensor_values[AS726X_NUM_CHANNELS];
	float calibrated_values[AS726X_NUM_CHANNELS];

	i2c_fd=init_i2c("/dev/i2c-1");
	if (i2c_fd < 0) {
		fprintf(stderr,"Error opening device!\n");
		return -1;
	}

	/* Init AS726X */
	if (ioctl(i2c_fd, I2C_SLAVE, AS726X_I2C_ADDRESS) < 0) {
		fprintf(stderr,"Error setting i2c address %x\n",
			AS726X_I2C_ADDRESS);
		return -1;
	}

	/* Init AS726X */

	control_value=(1<<7);		// RST = 1;
	virtual_write(i2c_fd, AS726X_CONTROL_SETUP, control_value);
	//_control_setup.RST = 0;

	/* wait for it to boot up */
	/* how long?? Less than 1s seems too short? */
	usleep(1000000);

	/* try to read the version register */
	version = virtual_read(i2c_fd, AS726X_HW_VERSION);

	if (version != 0x40) {
		fprintf(stderr,"Error!  Unsupported version %x\n",version);
		return -1;
	}

	enable_interrupt(i2c_fd);

	set_led_drv_current(i2c_fd,LIMIT_12MA5);

	/* Turn off the DRV LED */
	led_drv_off(i2c_fd);

	set_integration_time(i2c_fd,50);

	set_gain(i2c_fd,GAIN_64X);

	set_conversion_type(i2c_fd,ONE_SHOT);


	memset(sensor_values,0,sizeof(sensor_values));
	memset(calibrated_values,0,sizeof(calibrated_values));

	/*****************************/
	/* actually do a measurement */
	/*****************************/

	while(1) {

	/* read the device temperature */
	temp = read_temperature(i2c_fd);

	/* set use_led if you want the LED on for reading */
	if (use_led) led_drv_on(i2c_fd);

	/* Start Measurement */
	start_measurement(i2c_fd);

	/* wait until data is avail */
	int ready=0;
	while (!ready) {
		usleep(5000);
		ready=data_ready(i2c_fd);
	}

	if (use_led) led_drv_off(i2c_fd);

	/* read the values */

	read_raw_values(i2c_fd,sensor_values,AS726X_NUM_CHANNELS);

	read_calibrated_values(i2c_fd,calibrated_values,AS726X_NUM_CHANNELS);

	printf("Temperature: %d C\n\n",temp);

	printf("Raw Values:\n");
	printf("\tViolet: %d\n",sensor_values[AS726X_VIOLET]);
	printf("\tBlue:   %d\n",sensor_values[AS726X_BLUE]);
	printf("\tGreen:  %d\n",sensor_values[AS726X_GREEN]);
	printf("\tYellow: %d\n",sensor_values[AS726X_YELLOW]);
	printf("\tOrange: %d\n",sensor_values[AS726X_ORANGE]);
	printf("\tRed:    %d\n\n",sensor_values[AS726X_RED]);

	printf("Calibrated Values:\n");
	printf("\tViolet: %f\n",calibrated_values[AS726X_VIOLET]);
	printf("\tBlue:   %f\n",calibrated_values[AS726X_BLUE]);
	printf("\tGreen:  %f\n",calibrated_values[AS726X_GREEN]);
	printf("\tYellow: %f\n",calibrated_values[AS726X_YELLOW]);
	printf("\tOrange: %f\n",calibrated_values[AS726X_ORANGE]);
	printf("\tRed:    %f\n\n",calibrated_values[AS726X_RED]);

	sleep(2);
	}

	return result;
}
