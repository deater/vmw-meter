#include <stdint.h>

#include "stm32l476xx.h"
#include "i2c.h"


void i2c_init(I2C_TypeDef *I2Cx) {

	uint32_t own_addr=0x52;	/* ??? */

	RCC->APB1ENR1 |= RCC_APB1ENR1_I2C1EN;	/* I2C1 clock enable */
	RCC->CCIPR &= ~RCC_CCIPR_I2C1SEL;
	RCC->CCIPR |= RCC_CCIPR_I2C1SEL_0;	/* select SYSCLK */

	RCC->APB1RSTR1 |= RCC_APB1RSTR1_I2C1RST;	/* reset i2c1 */
	RCC->APB1RSTR1 &= ~RCC_APB1RSTR1_I2C1RST;	/* finish reset */

	/* i2c CR1 Config */
	/* When i2c is disabled (PE=0) the i2c hardware is reset */

	I2Cx->CR1 &= ~I2C_CR1_PE;		/* Disable i2c */
	I2Cx->CR1 &= ~I2C_CR1_ANFOFF;		/* Enable analog noise filter */
	I2Cx->CR1 &= ~I2C_CR1_DNF;		/* Digital filter disable */
	I2Cx->CR1 |= I2C_CR1_ERRIE;		/* Error interrupt enable */
	//I2Cx->CR1 &= ~I2C_CR1_SMBUS;		/* SMBUS: 0=i2c, 1=smbus */
						/* not in the manual? */
	I2Cx->CR1 &= ~I2C_CR1_NOSTRETCH;	/* Enable clock stretching */

	/* Set up i2c timing.  Assuming systimer set to 80MHz */
	I2Cx->TIMINGR=0;
	/* SysTimer=80MHz, PRESC=7, so 80MHz/(1+7) = 10 MHz */
	I2Cx->TIMINGR &= ~I2C_TIMINGR_PRESC;	/* Clear prescaler */
	I2Cx->TIMINGR |= 7U << 28;		/* Set prescaler to 7 */
	I2Cx->TIMINGR |= 49U;		/* SCL low period (master) > 4.7 us */
	I2Cx->TIMINGR |= 49U<<8;	/* SCL high period (master0 > 4.0 us */
	I2Cx->TIMINGR |= 14U<<20;	/* Data setup time > 1.0 us */
	I2Cx->TIMINGR |= 15U<<16;	/* Data hold time > 1.25 us */

	/* Own address 1 register */
	I2Cx->OAR1 &= ~I2C_OAR1_OA1EN;
	I2Cx->OAR1 = I2C_OAR1_OA1EN | own_addr;	/* 7-bit own address */
	I2Cx->OAR1 &= ~I2C_OAR2_OA2EN;	/* disable own address 2 */

	/* i2c CR2 Config */
	I2Cx->CR2 &= ~I2C_CR2_ADD10;	/* 7-bit mode */
	I2Cx->CR2 |= I2C_CR2_AUTOEND;	/* Enable auto-end */
	I2Cx->CR2 |= I2C_CR2_NACK;	/* slave mode: set NACK */

	/* Enable I2c */
	I2Cx->CR1 |= I2C_CR1_PE;
}
