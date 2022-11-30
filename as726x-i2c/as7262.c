/* Code to access AS7262 6-channel visible light sensor */

/* Roughly based on code found in the adafruit arduino library */

#define AS726X_I2C_ADDRESS	0x49

#define AS726X_HW_VERSION	0x00
#define AS726X_FW_VERSION	0x02
#define AS726X_CONTROL_SETUP	0x04
#define AS726X_INT_T		0x05
#define AS726X_DEVICE_TEMP	0x06
#define AS726X_LED_CONTROL	0x07

#define AS726X_SLAVE_STATUS_REG	0x00
#define AS726X_SLAVE_WRITE_REG	0x01
#define AS726X_SLAVE_READ_REG	0x02
#define AS726X_SLAVE_TX_VALID	0x02
#define	AS726X_SLAVE_RX_VALID	0x01


#define	LIMIT_1MA	0x00	/* default */
#define	LIMIT_2MA	0x01
#define	LIMIT_4MA	0x02
#define	LIMIT_8MA	0x03

#define	LIMIT_12MA5	0x00	/* default */
#define	LIMIT_25MA	0x01
#define	LIMIT_50MA	0x02
#define	LIMIT_100MA	0x03

#define	GAIN_1X		0x00	/* default */
#define	GAIN_3X7	0x01
#define	GAIN_16X	0x02
#define	GAIN_64X	0x03

/* Conversion Types */
#define MODE_0		0x00
#define MODE_1		0x01
#define MODE_2		0x02	/* default */
#define ONE_SHOT	0x03

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>

#include <math.h>

#include <sys/time.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

#include "i2c_lib.h"

static int debug=1;


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
#define CONTROL_RST_MASK	1<<7
#define CONTROL_INT_MASK	1<<6
#define CONTROL_GAIN_MASK	3<<4
#define CONTROL_BANK_MASK	3<<2

static void enable_interrupt(int i2c_fd) {

	control_value|=(1<<6);	// INT = 1;
	virtual_write(i2c_fd, AS726X_CONTROL_SETUP, control_value);
}

static void set_gain(int i2c_fd, uint8_t gain) {

	control_value&=~CONTROL_GAIN_MASK;
	control_value|=gain<<4;
	virtual_write(i2c_fd,AS726X_CONTROL_SETUP, control_value);
}

void set_conversion_type(int i2c_fd, uint8_t type) {

	control_value&=~CONTROL_BANK_MASK;
	control_value|=type<<2;
	virtual_write(i2c_fd,AS726X_CONTROL_SETUP, control_value);
}

static uint8_t led_control_value=0x0;
#define LED_ICL_DRV_MASK	0x30
#define LED_DRV_MASK		0x80

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

static void set_integration_time(int i2c_fd,uint8_t time) {

	uint8_t int_value=time;

	virtual_write(i2c_fd,AS726X_INT_T, int_value);
}


int main(int argc, char **argv) {

	signed int result;

	int i2c_fd;
//	unsigned char buffer[16];
	uint8_t version,control_value;

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


	return result;
}

