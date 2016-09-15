/* Hooked up to Raspberry Pi via 74HC595 */
/* DS GPIO17     */
/* SHCP GPIO27   */
/* STCP GPIO22   */
/* BC1  GPIO18   */
/* BDIR GPIO23   */
/* |RESET GPIO24 */

#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>

#include <bcm2835.h>

#include "ay-3-8910.h"

#define DELAY_PAUSE	3
#define DELAY_BETWEEN	3
#define DELAY_SHIFT	10
#define DELAY_RESET	5000

#define AY38910_CLOCK	1000000	/* 1MHz on our board */

int initialize_ay_3_8910(void) {

	int result;

	printf("Initializing AY-3-8910\n");

	result=bcm2835_init();
	/* 1 means success, 0 means failure.  Who does that? */
	if (result==0) {
		fprintf(stderr,"Error init libBCM2835!\n");
		return -1;
	}

	/* Enable GPIO17 */
	bcm2835_gpio_fsel(RPI_V2_GPIO_P1_11, BCM2835_GPIO_FSEL_OUTP);
	/* Enable GPIO27 */
	bcm2835_gpio_fsel(RPI_V2_GPIO_P1_13, BCM2835_GPIO_FSEL_OUTP);
	/* Enable GPIO22 */
	bcm2835_gpio_fsel(RPI_V2_GPIO_P1_15, BCM2835_GPIO_FSEL_OUTP);
	/* Enable GPIO18 */
	bcm2835_gpio_fsel(RPI_V2_GPIO_P1_12, BCM2835_GPIO_FSEL_OUTP);
	/* Enable GPIO23 */
	bcm2835_gpio_fsel(RPI_V2_GPIO_P1_16, BCM2835_GPIO_FSEL_OUTP);
	/* Enable GPIO24 */
	bcm2835_gpio_fsel(RPI_V2_GPIO_P1_18, BCM2835_GPIO_FSEL_OUTP);

	/* Pull reset low than high */
	printf("Resetting...\n");
	bcm2835_gpio_write(RPI_GPIO_P1_18, LOW);
	bcm2835_delayMicroseconds(DELAY_RESET);
	bcm2835_gpio_write(RPI_GPIO_P1_18, HIGH);

	return 0;
}

static int shift_74hc595(int value, int size) {


	int i,shifted_val;

	/* Shift first 8 bits */

	shifted_val=value;

	for(i=0;i<8;i++) {
		/* Set clock low */
		bcm2835_gpio_write(27, LOW);

		bcm2835_delayMicroseconds(DELAY_SHIFT);

		if (shifted_val&0x1) {
			bcm2835_gpio_write(RPI_V2_GPIO_P1_11, HIGH);
		}
		else {
			bcm2835_gpio_write(RPI_V2_GPIO_P1_11, LOW);
		}
		shifted_val>>=1;

		bcm2835_delayMicroseconds(DELAY_SHIFT);

		/* Set clock high */
		bcm2835_gpio_write(27, HIGH);

		bcm2835_delayMicroseconds(DELAY_SHIFT);
	}


	/* Shift next 8 bits */
	if (size==16) {
	shifted_val=value;

	for(i=0;i<8;i++) {
		/* Set clock low */
		bcm2835_gpio_write(27, LOW);

		bcm2835_delayMicroseconds(DELAY_SHIFT);

		if (shifted_val&0x1) {
			bcm2835_gpio_write(RPI_V2_GPIO_P1_11, HIGH);
		}
		else {
			bcm2835_gpio_write(RPI_V2_GPIO_P1_11, LOW);
		}
		shifted_val>>=1;

		bcm2835_delayMicroseconds(DELAY_SHIFT);

		/* Set clock high */
		bcm2835_gpio_write(27, HIGH);

		bcm2835_delayMicroseconds(DELAY_SHIFT);
	}
	}

	bcm2835_gpio_write(22, HIGH);

	bcm2835_delayMicroseconds(DELAY_SHIFT);

	bcm2835_gpio_write(22, LOW);

	bcm2835_delayMicroseconds(DELAY_SHIFT);

	return 0;
}

int write_ay_3_8910(int addr, int value, int shift_size) {


	/* Be sure BDIR and BC1 are low */
	bcm2835_gpio_write(RPI_GPIO_P1_16, LOW);
	bcm2835_gpio_write(RPI_GPIO_P1_12, LOW);

	/* Set address on bus */
	shift_74hc595(addr,shift_size);

	/* Set BDIR and BC1 high */
	bcm2835_gpio_write(RPI_GPIO_P1_16, HIGH);
	bcm2835_gpio_write(RPI_GPIO_P1_12, HIGH);

	/* pause */
	bcm2835_delayMicroseconds(DELAY_PAUSE);

	/* Set BDIR and BC1 low */
	bcm2835_gpio_write(RPI_GPIO_P1_16, LOW);
	bcm2835_gpio_write(RPI_GPIO_P1_12, LOW);

	bcm2835_delayMicroseconds(DELAY_BETWEEN);

	/* Be sure BDIR and BC1 are low */
	/* Put value on bus */
	shift_74hc595(value,shift_size);

	/* Put BDIR high */
	bcm2835_gpio_write(RPI_GPIO_P1_16, HIGH);

	/* pause */
	bcm2835_delayMicroseconds(DELAY_PAUSE);

	/* Put BDIR low */
	bcm2835_gpio_write(RPI_GPIO_P1_16, LOW);

	bcm2835_delayMicroseconds(DELAY_BETWEEN);

	return 0;
}

void quiet_ay_3_8910(int shift_size) {

	int j;

	for(j=0;j<14;j++) {
		write_ay_3_8910(j,0,shift_size);
	}
}

