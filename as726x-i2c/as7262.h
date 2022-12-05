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

#define AS726X_INTEGRATION_TIME_MULT 2.8/* multiplier for integration time */
#define AS726X_NUM_CHANNELS 6		/* number of sensor channels */

#define AS726X_VIOLET	0
#define AS726X_BLUE	1
#define AS726X_GREEN	2
#define AS726X_YELLOW	3
#define AS726X_ORANGE	4
#define AS726X_RED	5

/* Color Registers */
#define AS7262_VIOLET		0x08
#define AS7262_BLUE		0x0A
#define AS7262_GREEN		0x0C
#define AS7262_YELLOW		0x0E
#define AS7262_ORANGE		0x10
#define AS7262_RED		0x12
#define AS7262_VIOLET_CALIBRATED	0x14
#define AS7262_BLUE_CALIBRATED		0x18
#define AS7262_GREEN_CALIBRATED		0x1C
#define AS7262_YELLOW_CALIBRATED	0x20
#define AS7262_ORANGE_CALIBRATED	0x24
#define AS7262_RED_CALIBRATED		0x28

#define CONTROL_RST_MASK	1<<7
#define CONTROL_INT_MASK	1<<6
#define CONTROL_GAIN_MASK	3<<4
#define CONTROL_BANK_MASK	3<<2
#define CONTROL_DATA_RDY_MASK	1<<1

#define LED_ICL_DRV_MASK	3<<4
#define LED_DRV_MASK		1<<3
