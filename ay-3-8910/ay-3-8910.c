/* Hooked up to Raspberry Pi via 74HC595 */

/* For GPIO	*/
/* DS	GPIO17 (pin11)	*/
/* SHCP GPIO27 (pin13)	*/
/* STCP GPIO22 (pin15)	*/

#if 1
#define DS	17
#define	SH	27
#define ST	22
#else
#define DS	10
#define	SH	11
#define ST	7
#endif


/* For SPI	*/
/* DS 	MOSI/GPIO10 (pin19)	*/
/* SHCP CLK/GPIO11 (pin23)	*/
/* STCP	CE1/GPIO7 (pin26)	*/

/* For either */
/* BC1  GPIO18   */
/* BDIR GPIO23   */
/* |RESET GPIO24 */

static int use_gpio=1;

#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <sys/ioctl.h>

#include <bcm2835.h>

#include "ay-3-8910.h"

#define DELAY_PAUSE	3
#define DELAY_BETWEEN	3
#define DELAY_SHIFT	10
#define DELAY_RESET	5000

#define AY38910_CLOCK	1000000	/* 1MHz on our board */

static int gpio_shift_74hc595(int value, int size) {


	int i,shifted_val;

	/* Shift first 8 bits */

	shifted_val=value;

	for(i=0;i<8;i++) {
		/* Set clock low */
		bcm2835_gpio_write(SH, LOW);

		bcm2835_delayMicroseconds(DELAY_SHIFT);

		if (shifted_val&0x1) {
			bcm2835_gpio_write(DS, HIGH);
		}
		else {
			bcm2835_gpio_write(DS, LOW);
		}
		shifted_val>>=1;

		bcm2835_delayMicroseconds(DELAY_SHIFT);

		/* Set clock high */
		bcm2835_gpio_write(SH, HIGH);

		bcm2835_delayMicroseconds(DELAY_SHIFT);
	}


	/* Shift next 8 bits */
	if (size==16) {
	shifted_val=value;

	for(i=0;i<8;i++) {
		/* Set clock low */
		bcm2835_gpio_write(SH, LOW);

		bcm2835_delayMicroseconds(DELAY_SHIFT);

		if (shifted_val&0x1) {
			bcm2835_gpio_write(DS, HIGH);
		}
		else {
			bcm2835_gpio_write(DS, LOW);
		}
		shifted_val>>=1;

		bcm2835_delayMicroseconds(DELAY_SHIFT);

		/* Set clock high */
		bcm2835_gpio_write(SH, HIGH);

		bcm2835_delayMicroseconds(DELAY_SHIFT);
	}
	}

	bcm2835_gpio_write(ST, HIGH);

	bcm2835_delayMicroseconds(DELAY_SHIFT);

	bcm2835_gpio_write(ST, LOW);

	bcm2835_delayMicroseconds(DELAY_SHIFT);

	return 0;
}

static int spi_shift_74hc595(int value, int size) {

	int result=0;
	unsigned char data_out[2];

	/* Reverse bits */
	/* On pi SPI it is MSB first only */
	unsigned char b = value;
	b = ((b * 0x0802LU & 0x22110LU) | (b * 0x8020LU & 0x88440LU)) * 0x10101LU >> 16; 

	/* For now just duplicate */
	data_out[0]=b;
	data_out[1]=0;

	bcm2835_spi_writenb((char *)data_out,2);

	return result;
}

int write_ay_3_8910(int addr, int value, int shift_size) {

	/* Be sure BDIR and BC1 are low */
	bcm2835_gpio_write(RPI_GPIO_P1_16, LOW);
	bcm2835_gpio_write(RPI_GPIO_P1_12, LOW);

	/* Set address on bus */
	if (use_gpio) gpio_shift_74hc595(addr,shift_size);
	else spi_shift_74hc595(addr,shift_size);

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
	if (use_gpio) gpio_shift_74hc595(value,shift_size);
	else spi_shift_74hc595(value,shift_size);

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

int initialize_ay_3_8910(int output_use_gpio) {

	int result;

	use_gpio=output_use_gpio;

	printf("Initializing AY-3-8910, using %s\n",use_gpio?"GPIO":"SPI");

	result=bcm2835_init();
	/* 1 means success, 0 means failure.  Who does that? */
	if (result==0) {
		fprintf(stderr,"Error init libBCM2835!\n");
		return -1;
	}

	if (use_gpio) {
		/* Enable GPIO17 */
		bcm2835_gpio_fsel(DS, BCM2835_GPIO_FSEL_OUTP);
		/* Enable GPIO27 */
		bcm2835_gpio_fsel(SH, BCM2835_GPIO_FSEL_OUTP);
		/* Enable GPIO22 */
		bcm2835_gpio_fsel(ST, BCM2835_GPIO_FSEL_OUTP);
	}
	else {
		bcm2835_spi_begin();
		bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_4096 );
//		bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_16384 );
//		bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_32768 );
		bcm2835_spi_chipSelect(BCM2835_SPI_CS1);
		bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS1,0);
		bcm2835_spi_setDataMode (BCM2835_SPI_MODE0);

		/* Doesn't work, always is MSB first */
		//bcm2835_spi_setBitOrder (BCM2835_SPI_BIT_ORDER_LSBFIRST);
	}

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

void close_ay_3_8910(void) {

	if (use_gpio) {
	}
	else {
		bcm2835_spi_end();
	}
}
