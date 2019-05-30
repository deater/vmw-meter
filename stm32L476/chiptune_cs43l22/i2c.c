#include <stdint.h>

#include "stdlib.h"

#include "stm32l476xx.h"
#include "i2c.h"


void i2c_init(I2C_TypeDef *I2Cx) {

	uint32_t own_addr=0x52;	/* ??? */
	int pin1=6,pin2=7;

	if (I2Cx==I2C1) {

		RCC->APB1ENR1 |= RCC_APB1ENR1_I2C1EN;	/* I2C1 clock enable */
		RCC->CCIPR &= ~RCC_CCIPR_I2C1SEL;
		RCC->CCIPR |= RCC_CCIPR_I2C1SEL_0;	/* select SYSCLK */

		RCC->APB1RSTR1 |= RCC_APB1RSTR1_I2C1RST;	/* reset i2c1 */
		RCC->APB1RSTR1 &= ~RCC_APB1RSTR1_I2C1RST;	/* finish reset */


		/* I2C1 GPIO Configuration */
		/* See UM1879.book p 32 */
		/* I2C1_SCL = PB6 */
		/* I2C1_SDA = PB7 */

		RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;

		/* Set mode of pins as alternate function */
		/* 00 = input, 01 = output, 10 = alternate, 11 = analog (def) */
		GPIOB->MODER &= ~(3UL<<(pin1*2));
		GPIOB->MODER |= 2UL<<(pin1*2);
		GPIOB->MODER &= ~(3UL<<(pin2*2));
		GPIOB->MODER |= 2UL<<(pin2*2);

		/* set alternate function 4 (I2C1_SCL/I2C1_SDA) */
		/* this is AF0 =  (appendix I of book) */
		GPIOB->AFR[0] &= ~0x0f000000;
		GPIOB->AFR[0] |=  0x04000000;
		GPIOB->AFR[0] &= ~0xf0000000;
		GPIOB->AFR[0] |=  0x40000000;

		/* Set as no pull-up/pull-down */
		/* 00 = no pull-up, no pull-down, 01 = pull-up, 10 = pull-down */
		GPIOB->PUPDR &=~(3UL<<(pin1*2));
		GPIOB->PUPDR &=~(3UL<<(pin2*2));

		/* Set output as open drain */
		GPIOB->OTYPER |= (1<<pin1);
		GPIOB->OTYPER |= (1<<pin2);

		/* Set speed to fast */
		GPIOB->OSPEEDR |= (2<<(pin1*2));
		GPIOB->OSPEEDR |= (2<<(pin2*2));


	}

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

void i2c_wait_line_idle(I2C_TypeDef *I2Cx) {
	/* wait until i2c line is ready */
	while ( (I2Cx->ISR & I2C_ISR_BUSY)==I2C_ISR_BUSY);
}

unsigned int krg=0;

void i2c_start(I2C_TypeDef *I2Cx, uint32_t dev_addr,
		uint8_t size, uint8_t direction) {

	/* direction==0, master wants to write */
	/* direction==1, master requests a read */

	uint32_t tmpreg=I2Cx->CR2;

	tmpreg &=~I2C_CR2_SADD;		// clear out address
	tmpreg &=~I2C_CR2_NBYTES;	// clear out number of bytes
	tmpreg &=~I2C_CR2_RELOAD;	// reload=0, transfer ends after N bytes
	tmpreg &=~I2C_CR2_AUTOEND;	// 0, do not auto-send stop when done
	tmpreg &=~I2C_CR2_RD_WRN;	// clear read/write
	tmpreg &=~I2C_CR2_START;	// clear start flag
	tmpreg &=~I2C_CR2_STOP;		// clear stop flag
	tmpreg &=~I2C_CR2_ADD10;	// 7-bit address

	if (direction==I2C_READ_FROM_SLAVE) {
		tmpreg |= I2C_CR2_RD_WRN;	// 1=read
	}
	else {
		tmpreg &= ~I2C_CR2_RD_WRN;	// 0=write
	}

	tmpreg |= (dev_addr & I2C_CR2_SADD);		// set address
	tmpreg |= ((size<<16) & I2C_CR2_NBYTES);	// set num bytes

	tmpreg |= I2C_CR2_START;			// set start flag

	krg=tmpreg;

	I2Cx->CR2 = tmpreg;

}

void i2c_stop(I2C_TypeDef *I2Cx) {

	/* Generate STOP bit after the current byte has been transferred */
	I2Cx->CR2 |= I2C_CR2_STOP;

	/* Wait until STOPF flag is reset */
	while( (I2Cx->ISR & I2C_ISR_STOPF)==0);

	/* clear stopf flag */
	I2Cx->ICR |= I2C_ICR_STOPCF;

}



int8_t	i2c_send_data(I2C_TypeDef *I2Cx, uint8_t slave_addr,
		uint8_t *data, uint8_t size) {

	int i;

	if ((size<=0) || (data==NULL)) return -1;

	/* Wait until line is idle */
	i2c_wait_line_idle(I2Cx);

	i2c_start(I2Cx, slave_addr, size, I2C_WRITE_TO_SLAVE);

	for(i=0;i<size;i++) {
		/* TXIS bit set by hardware when TXDR is empty */
		/* and we must write new data to TXDR. */
		/* It is cleared when we write data. */
		/* It is not set when a NACK is received */

		while((I2Cx->ISR & I2C_ISR_TXIS)==0);

		I2Cx->TXDR = data[i]&I2C_TXDR_TXDATA;
	}

	/* Wait until the TC flag is set */
	while(  ((I2Cx->ISR & I2C_ISR_TC)==0) &&
		((I2Cx->ISR & I2C_ISR_NACKF)==0) );


	if ( (I2Cx->ISR & I2C_ISR_NACKF)!=0) {
		return -1;
	}

	i2c_stop(I2Cx);

	return 0;
}

int8_t i2c_receive_data(I2C_TypeDef *I2Cx, uint8_t slave_addr,
		uint8_t *data, uint8_t size) {

	int i;

	if ((size<=0) || (data==NULL)) return -1;

	i2c_wait_line_idle(I2Cx);

	i2c_start(I2Cx, slave_addr, size, I2C_READ_FROM_SLAVE);

	for(i=0;i<size;i++) {
		/* wait until RXNE flag is set */
		while( (I2Cx->ISR & I2C_ISR_RXNE)==0);
		data[i]=I2Cx->RXDR & I2C_RXDR_RXDATA;
	}

	while((I2Cx->ISR & I2C_ISR_TC)==0); /* Wait until TC flag set */

	i2c_stop(I2Cx);

	return 0;
}
