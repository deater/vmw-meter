#include <stdint.h>

#include "i2c.h"

#include "write_display.h"

#define HT16K33_REGISTER_ADDRESS_POINTER        0x00
#define HT16K33_REGISTER_SYSTEM_SETUP           0x20
#define HT16K33_REGISTER_KEY_DATA_POINTER       0x40
#define HT16K33_REGISTER_INT_ADDRESS_POINTER    0x60
#define HT16K33_REGISTER_DISPLAY_SETUP          0x80
#define HT16K33_REGISTER_ROW_INT_SET            0xA0
#define HT16K33_REGISTER_TEST_MODE              0xD0
#define HT16K33_REGISTER_DIMMING                0xE0

/* Blink rate */
#define HT16K33_BLINKRATE_OFF                   0x00
#define HT16K33_BLINKRATE_2HZ                   0x01
#define HT16K33_BLINKRATE_1HZ                   0x02
#define HT16K33_BLINKRATE_HALFHZ                0x03

#define HT16K33_ADDRESS		0x70

void init_display(struct i2c_inst_t *i2c) {

	unsigned char buffer[17];

	/* Turn on oscillator */

	buffer[0]= HT16K33_REGISTER_SYSTEM_SETUP | 0x01;
	i2c_write_byte(i2c,HT16K33_ADDRESS,buffer[0]);

	/* Turn on Display, No Blink */

	buffer[0]= HT16K33_REGISTER_DISPLAY_SETUP | HT16K33_BLINKRATE_OFF | 0x1;
	i2c_write_byte(i2c,HT16K33_ADDRESS,buffer[0]);

	/* Set Brightness */
	int value=12;

//	if ((value<0) || (value>15)) {
//		fprintf(stderr,"Brightness value of %d out of range (0-15)\n",value);
//		return -1;
//	}

	buffer[0]= HT16K33_REGISTER_DIMMING | value;
	i2c_write_byte(i2c,HT16K33_ADDRESS,buffer[0]);

}

static int number_lookup[10]={
	0x3f,	/* 0 */
	0x06,	/* 1 */
	0x5b,	/* 2 */
	0x4f,	/* 3 */
	0x66,	/* 4 */
	0x6d,	/* 5 */
	0x7d,	/* 6 */
	0x07,	/* 7 */
	0x7f,	/* 8 */
	0x67,	/* 9 */
};

void write_display(struct i2c_inst_t *i2c, uint32_t value) {

	unsigned char buffer[17];

	uint8_t ones,tens,hundreds,thousands;

	ones=value%10;
	value/=10;
	tens=value%10;
	value/=10;
	hundreds=value%10;
	value/=10;
	thousands=value%10;


	/* Write 1s to all Display Columns */
	int i;
	for(i=0;i<16;i++) {
		buffer[i]=0;
	}

	buffer[0]=0x00;	/* start at beginning */
	buffer[1]=number_lookup[thousands];	/* digit 1 */
	buffer[3]=number_lookup[hundreds];	/* digit 2 */
	buffer[5]=0x00;			/* colon */
	buffer[7]=number_lookup[tens];	/* digit 3 */
	buffer[9]=number_lookup[ones];	/* digit 4  */

	for(i=0;i<17;i++) {
		i2c_write_blocking(i2c,HT16K33_ADDRESS,buffer,17,0);
	}

}



void shutdown_display(void) {

}
