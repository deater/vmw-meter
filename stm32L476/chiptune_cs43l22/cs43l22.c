#include <stdint.h>

#include "stm32l476xx.h"
#include "i2c.h"

void cs43l22_init(void) {

	volatile int delay;

	/* PB6/PB7 is SCL/SDA.  Those are initialized in i2c.c */
	/* PE3 is Reset */
	/* PE2/PE5/PE6/PE4 is SAI1 MCK/SCK/SD/FS */

	/* Enable GPIOE */
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOEEN;

	/* p31 of document */

	/* Hold |reset low until power supplies are stable */

	GPIOE->ODR&=~(1<<3);
	for(delay=0;delay<1000;delay++);

	/* Bring |reset high */
	GPIOE->ODR|= (1<<3);
	for(delay=0;delay<1000;delay++);

	/* 4.11 required settings */

	/* Write 0x99 to register 0x00 */

	/* Write 0x80 to register 0x47 */

	/* Write 1 to bit 7 in register 0x32 */

	/* Write 0 to bit 7 in register 0x32 */

	/* Write 0 to regsiter 0x00 */


}


void cs43l22_disable(void) {

//	volatile int delay;

	/* p31 of document */

	/* Mute DAC/PWM */
	/* Disable soft ramp and zero cross volume */
	/* Set "Power Ctl 1" register (0x2) to 0x9f */
	/* Wait at least 100us */
	/* Turn off MCLK */
	/* Bring reset low */

}

