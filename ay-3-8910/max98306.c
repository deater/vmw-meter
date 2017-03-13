#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include "max98306.h"
#include <bcm2835.h>


#define SHUTDOWN_GPIO	21
#define HEADPHONE_GPIO	20
#define GAIN_GPIO	12
#define GAIN_PRIME_GPIO	13


int max98306_init(void) {

	/* Initialize the GPIOs */

	/************/
	/* SHUTDOWN */
	/************/

	bcm2835_gpio_fsel(SHUTDOWN_GPIO,  BCM2835_GPIO_FSEL_OUTP);
	bcm2835_gpio_fsel(HEADPHONE_GPIO, BCM2835_GPIO_FSEL_INPT);
	bcm2835_gpio_fsel(GAIN_GPIO, BCM2835_GPIO_FSEL_INPT);
	bcm2835_gpio_fsel(GAIN_PRIME_GPIO, BCM2835_GPIO_FSEL_INPT);

	/* Set volume to low as default */
	max98306_set_volume(1);

	return 0;
}

int max98306_check_headphone(void) {

	int value=0;

	value=bcm2835_gpio_lev(HEADPHONE_GPIO);

	return value;

}

#define GAIN_LOW	0
#define GAIN_HIGH	1
#define GAIN_FLOAT	2

int max98306_set_volume(int value) {

	int volume;
	int gain_setting,gain_prime_setting;

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
	if (gain_setting==GAIN_FLOAT) {
		/* Set float by making an INPUT */
		bcm2835_gpio_fsel(GAIN_GPIO, BCM2835_GPIO_FSEL_INPT);
	}
	else {
		bcm2835_gpio_fsel(GAIN_GPIO, BCM2835_GPIO_FSEL_OUTP);
		if (gain_setting==GAIN_HIGH) {
			bcm2835_gpio_set(GAIN_GPIO);
		}
		else {
			bcm2835_gpio_clr(GAIN_GPIO);
		}
	}

	/* Set GAIN_PRIME value */
	if (gain_prime_setting==GAIN_FLOAT) {
		/* Set float by making an INPUT */
		bcm2835_gpio_fsel(GAIN_PRIME_GPIO, BCM2835_GPIO_FSEL_INPT);
	}
	else {
		bcm2835_gpio_fsel(GAIN_PRIME_GPIO, BCM2835_GPIO_FSEL_OUTP);
		if (gain_setting==GAIN_HIGH) {
			bcm2835_gpio_set(GAIN_PRIME_GPIO);
		}
		else {
			bcm2835_gpio_clr(GAIN_PRIME_GPIO);
		}

	}

	return 0;
}


int max98306_disable(void) {

	 bcm2835_gpio_clr(SHUTDOWN_GPIO);

	return 0;
}

int max98306_enable(void) {

	 bcm2835_gpio_set(SHUTDOWN_GPIO);

	return 0;
}


int max98306_free(void) {

	max98306_disable();

	return 0;
}
