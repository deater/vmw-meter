#include <stdint.h>

#include "stm32l476xx.h"
#include "i2c.h"

void cs43l22_init(void) {

	volatile int delay;
	int slave_addr=0x94;
	unsigned char sound_data[2];
	int reset_pin=3;
	int mclk_pin=2,sclk_pin=5,sd_pin=6;

	/* PB6/PB7 is SCL/SDA.  Those are initialized in i2c.c */
	/* PE3 is Reset */
	/* PE2/PE5/PE6/PE4 is SAI1 MCK/SCK/SD/FS */

	/* Enable GPIOE */
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOEEN;

	/* Set to output */
	GPIOE->MODER &= ~(3UL<<(reset_pin*2));
	GPIOE->MODER |= 1UL<<(reset_pin*2);


	/* p31 of document */

	/* Hold |reset low until power supplies are stable */

	GPIOE->ODR&=~(1<<reset_pin);
	for(delay=0;delay<4000;delay++);

	/* Bring |reset high */
	GPIOE->ODR|= (1<<reset_pin);
	for(delay=0;delay<4000;delay++);

	/* 4.11 required settings */

	/* Write 0x99 to register 0x00 */
	sound_data[0]=0x00;
	sound_data[1]=0x99;
	i2c_send_data(I2C1,slave_addr,sound_data,2);

	/* Write 0x80 to register 0x47 */
	sound_data[0]=0x47;
	sound_data[1]=0x80;
	i2c_send_data(I2C1,slave_addr,sound_data,2);

	/* Write 1 to bit 7 in register 0x32 */
	sound_data[0]=0x32;
	sound_data[1]=0x80;
	i2c_send_data(I2C1,slave_addr,sound_data,2);

	/* Write 0 to bit 7 in register 0x32 */
	sound_data[0]=0x32;
	sound_data[1]=0x00;
	i2c_send_data(I2C1,slave_addr,sound_data,2);

	/* Write 0 to regsiter 0x00 */
	sound_data[0]=0x00;
	sound_data[1]=0x00;
	i2c_send_data(I2C1,slave_addr,sound_data,2);


	/* Enable the clock for SAI1 */
	RCC->APB2ENR |= RCC_APB2ENR_SAI1EN;

	/* Configure GPIOE for PE2(MCLK), PE5(SCLK) and PE6(SDIN) on I2S */
	/* PE4=fs??? */
	/* Alternate function 13 */

	/* Set mode of pins as alternate function */
	/* 00 = input, 01 = output, 10 = alternate, 11 = analog (def) */
	GPIOE->MODER &= ~(3UL<<(mclk_pin*2));
	GPIOE->MODER |= 2UL<<(mclk_pin*2);
	GPIOE->MODER &= ~(3UL<<(sclk_pin*2));
	GPIOE->MODER |= 2UL<<(sclk_pin*2);
	GPIOE->MODER &= ~(3UL<<(sd_pin*2));
	GPIOE->MODER |= 2UL<<(sd_pin*2);

	/* set alternate function 13 (SAI1) */
	/* (appendix I of book) */
	GPIOE->AFR[0] &= ~0x00000f00;
	GPIOE->AFR[0] |=  0x00000d00;
	GPIOE->AFR[0] &= ~0x00f00000;
	GPIOE->AFR[0] |=  0x00d00000;
	GPIOE->AFR[0] &= ~0x0f000000;
	GPIOE->AFR[0] |=  0x0d000000;

	/* Set as no pull-up/pull-down */
	/* 00 = no pull-up, no pull-down, 01 = pull-up, 10 = pull-down */
	GPIOE->PUPDR &=~(3UL<<(mclk_pin*2));
	GPIOE->PUPDR &=~(3UL<<(sclk_pin*2));
	GPIOE->PUPDR &=~(3UL<<(sd_pin*2));

	/* Set output as open drain */
	GPIOE->OTYPER |= (1<<mclk_pin);
	GPIOE->OTYPER |= (1<<sclk_pin);
	GPIOE->OTYPER |= (1<<sd_pin);

	/* Set speed to fast */
	GPIOE->OSPEEDR |= (2<<(mclk_pin*2));
	GPIOE->OSPEEDR |= (2<<(sclk_pin*2));
	GPIOE->OSPEEDR |= (2<<(sd_pin*2));

#if 0
    I2S_Params.Instance = SPI3; 
    I2S_Params.Init.Mode = I2S_MODE_MASTER_TX;
    I2S_Params.Init.Standard = I2S_STANDARD_PHILIPS;
    I2S_Params.Init.DataFormat = I2S_DATAFORMAT_16B;
    I2S_Params.Init.MCLKOutput = I2S_MCLKOUTPUT_ENABLE;
    I2S_Params.Init.AudioFreq = I2S_AUDIOFREQ_48K;
    I2S_Params.Init.CPOL = I2S_CPOL_LOW;
    //I2S_Params.Init.ClockSource = I2S_CLOCK_PLL;
    //I2S_Params.Init.FullDuplexMode = I2S_FULLDUPLEXMODE_DISABLE;
    HAL_I2S_Init(&I2S_Params);
#endif



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

