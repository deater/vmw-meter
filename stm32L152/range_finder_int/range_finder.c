/* Ultrasonic Range Finder */

/* For a HC-SR04 Ultrasonic Sensor */

/* It sends a 10us pulse to start ranging (3.3V tolerant) */
/* Then it receives back a 5V pulse from 150us-38ms       */
/*  the distance can be calculated based on the width of  */
/*  the pulse.  i.e. time/148 = distance in inches        */

/* This program hooks it to an STM32L discovery board     */
/* The pulse is sent over PB5 (disabling some segments    */
/*   on the LCD) using timer TIM2 in PWM one-pulse mode   */
/* The length is captured using TIM4 in input capture mode*/

#include <stdint.h>
#include "stm32l.h"

#include "delay_lib.h"
#include "lcd_lib.h"
#include "num_to_string.h"

static char string[7];

void convert_to_inches(int us) {

	int inches,feet;
	int length;
	char number[7];

	inches=us/148;

	feet=inches/12;
	inches=inches%12;

	length=num_to_string(number,feet);

	if (length==1) {
		string[0]=' ';
		string[1]=number[0];
	}
	else {
		string[0]=number[0];
		string[1]=number[1];
	}
	string[2]='\'';

	length=num_to_string(number,inches);

	if (length==1) {
		string[3]=' ';
		string[4]=number[0];
	}
	else {
		string[3]=number[0];
		string[4]=number[1];
	}
	string[5]='\"';

}

static TIM_TypeDef *tim4;
static int after=0,before=0,time_interval=0;


int main(void) {

	unsigned int lcd_buffer[16];

	GPIO_TypeDef *gpiob=(GPIO_TypeDef *)GPIOB_BASE;
	RCC_TypeDef *rcc=(RCC_TypeDef *)RCC_BASE;
	TIM_TypeDef *tim3=(TIM_TypeDef *)TIM3_BASE;
	tim4=(TIM_TypeDef *)TIM4_BASE;
	NVIC_ISER_TypeDef *nvic=(NVIC_ISER_TypeDef *)NVIC_ISER_BASE;
	uint32_t temp;

	/******************/
	/* configure LCD  */
	/******************/

	lcd_clock_init();

	lcd_pin_init();

	lcd_config();


	/*******************/
	/* Configure GPIOB */
	/*******************/

	/* Enable GPIOB */

	rcc->AHBENR |= AHBENR_GPIOBEN;	/* Enable GPIOB clock */

	/* Take over PB5 from LCD */

	/* Set TIM4 for pins PB6 and PB7 */
	temp=gpiob->MODER;
	temp&=~0x0000fc00;		/* clear values for pin 6 */
	temp|= (MODER_ALT<<(6*2));	/* Set output type to ALTERNATE */
	temp|= (MODER_ALT<<(7*2));	/* Set output type to ALTERNATE */
	temp|= (MODER_ALT<<(5*2));	/* Set output type to ALTERNATE */
	gpiob->MODER = temp; 		/* Set Output mode */

	gpiob->AFR[0] &= ~0xfff00000;

	gpiob->AFR[0] |= AFR_TIM345<<(4*5);
	gpiob->AFR[0] |= AFR_TIM345<<(4*6);
					/* Pin B6 = AF2 (TIM4) */
	gpiob->AFR[0] |= AFR_TIM345<<(4*7);
					/* Pin B7 = AF2 (TIM4) */
					/* i.e. hook up to TIM4 clock */


	/* Clock Speed */
	temp=gpiob->OSPEEDR&~0x0000fc00;
	temp|= 0x0000fc00;		/* 40MHz clock speed */
	gpiob->OSPEEDR=temp;

	/* Pull-Up/Pull-Down */
	gpiob->PUPDR &=~0x0000fc00;	/* No pull-up pull-down */

	gpiob->PUPDR |= 1<<(7*2);	/* PB7 pull up */
	gpiob->PUPDR |= 1<<(5*2);	/* PB7 pull up */

	/* Set output type */
	gpiob->OTYPER &=~0xc8;		/* Push-Pull */


	/******************/
	/* Set up Timer 3 */
	/******************/

	/* Enable Clock */
	rcc->APB1ENR |= RCC_APB1ENR_TIM3EN;

	/* MSI Clock is 2.097MHz */

	/* Want PWM mode */
	/* Cycle time set by ARR */
	/* Duty cycle in CCR1    */

	/* Divide by 2 to get 1.0485MHz */
	/* 0.953us			*/
	tim3->PSC = (2)-1;		/* Pre-scalar value */

	tim3->CCR2 = 11;		/* Load 5 into capture/compare 1 */
					/* */

	tim3->ARR = 20;			/* auto-reload register */
					/* causes overflow after 11 cycles */
					/* 10.5us */


	tim3->CR1 |= TIM_CR1_OPM;	/* One-pulse mode */
					/* Stop after hitting ARR first time */

	tim3->CCMR1 = 6<<12;		/* Use PWM mode 2 */
					/* chan1 low when CNT < CCR1 */
					/* chan1 high when CNT > CCR1  */

	tim3->CCER = TIM_CCER_CC2E;	/* Enable channel 2 output */

	tim3->CR1 |= TIM_CR1_CEN;	/* Enable timer */



	/******************/
	/* Set up Timer 4 */
	/******************/


	/* Enable Clock */
	rcc->APB1ENR |= RCC_APB1ENR_TIM4EN;


	/* MSI Clock is 2.097MHz */

	/* Want PWM mode */
	/* Cycle time set by ARR */
	/* Duty cycle in CCR1    */

	/* Divide by 21 to get roughly 100kHz	*/
	/* 10us					*/

	tim4->PSC = (21)-1;		/* Pre-scalar value */

	tim4->ARR = 0xffff;		/* auto-reload register */
					/* causes overflow after 11 cycles */
					/* 10.5us */

	tim4->CCMR1 = 1<<0;		/* Chan1 is input, IC1 is on TI1 */
					/* CC1P = 0 CC1NP=0 */
					/* active rising edge */

	tim4->CCMR1 |= 2<<4;		/* filter for 4 cycles */


	tim4->CCMR1 |=2<<8;		/* Chan2 is input IC2 is on TI1 */

	tim4->CCER  |=1<<5;		/* CC2P is 1, trigger on down edge */


	tim4->CCER |= TIM_CCER_CC1E|TIM_CCER_CC2E;
					/* Enable Channel 1 and 2  */

	/* Enable interrupts */
	tim4->DIER |= TIM_DIER_CC1IE;
	tim4->DIER |= TIM_DIER_CC2IE;

	//NVIC->IP[TIM4_IRQn] = tmppriority;
	/* Enable TIM4 interrupt in NVIC */
	nvic->ISER[TIM4_IRQn >> 0x05] =
		(uint32_t)0x01 << (TIM4_IRQn & (uint8_t)0x1F);

	/* Loop forever */

	tim4->CR1 |= TIM_CR1_CEN;	/* Enable timer */

	for(;;) {

		tim3->CR1 |= TIM_CR1_CEN;	/* Enable trigger */

		{int i; for(i=0;i<7;i++) string[i]=' ';}
//		num_to_string(string,after);

		convert_to_inches( (time_interval)*10 );
		lcd_convert(string,lcd_buffer);
                lcd_display(lcd_buffer);

		busy_delay(250000);



	}
}



static void tim4_irq_handler(void) {

	if (tim4->SR & TIM_SR_CC1F ) { // if update flag is set
		before=tim4->CCR1;
	}

	if (tim4->SR & TIM_SR_CC2F ) { // if update flag is set
		after = tim4->CCR2;

		if (before>after) {
			time_interval=(65536-before)+after;
		}
		else {
			time_interval = after - before;

		}
	}

	tim4->SR &= ~TIM_SR_UIF;
}


static void nmi_handler(void) {
	for(;;);
}

static void hardfault_handler(void) {
	for(;;);
}

static void do_nothing(void) {
	for(;;);
}


/* Vector Table */

#define STACK_TOP 0x20000800

/* CD00240193.pdf p 190 */

unsigned int *myvectors[]
__attribute__ ((section("vectors"))) = {
	(uint32_t *) STACK_TOP,		/* 0x00 stack pointer      */
	(uint32_t *) main,		/* 0x04 code entry point   */
	(uint32_t *) nmi_handler,	/* 0x08 NMI handler        */
	(uint32_t *) hardfault_handler,	/* 0x0c hard fault handler */
	(uint32_t *) do_nothing,	/* 0x10 mem_manage */
	(uint32_t *) do_nothing,	/* 0x14 bus_fault */
	(uint32_t *) do_nothing,	/* 0x18 usage_fault */
	(uint32_t *) 0,			/* 0x1c reserved */
	(uint32_t *) 0,			/* 0x20 reserved */
	(uint32_t *) 0,			/* 0x24 reserved */
	(uint32_t *) 0,			/* 0x28 reserved */
	(uint32_t *) do_nothing,	/* 0x2c SVC */
	(uint32_t *) do_nothing,	/* 0x30 debug */
	(uint32_t *) 0,			/* 0x34 reserved */
	(uint32_t *) do_nothing,	/* 0x38 pend */
	(uint32_t *) do_nothing,	/* 0x3c timer tick */
	/* External Interrupts */
	(uint32_t *) 0,			/* 0x40  Window Watchdog */
	(uint32_t *) 0,			/* 0x44 PVD through EXTI Line detect */
	(uint32_t *) 0,			/* 0x48 Tamper and Time Stamp */
	(uint32_t *) 0,			/* 0x4c RTC Wakeup */
	(uint32_t *) 0,			/* 0x50 FLASH */
	(uint32_t *) 0,			/* 0x54  RCC */
	(uint32_t *) 0,			/* 0x58 EXTI Line 0 */
	(uint32_t *) 0,			/* 0x5c EXTI Line 1 */
	(uint32_t *) 0,			/* 0x60 EXTI Line 2 */
	(uint32_t *) 0,			/* 0x64 EXTI Line 3 */
	(uint32_t *) 0,			/* 0x68 EXTI Line 4 */
	(uint32_t *) 0,			/* 0x6c DMA1 Channel */
	(uint32_t *) 0,			/* 0x70 DMA1 Channel 2 */
	(uint32_t *) 0,			/* 0x74 DMA1 Channel 3 */
	(uint32_t *) 0,			/* 0x78 DMA1 Channel 4 */
	(uint32_t *) 0,			/* 0x7c DMA1 Channel 5 */
	(uint32_t *) 0,			/* 0x80 DMA1 Channel 6 */
	(uint32_t *) 0,			/* 0x84 DMA1 Channel 7 */
	(uint32_t *) 0,			/* 0x88 ADC1 */
	(uint32_t *) 0,			/* 0x8c USB High Priority */
	(uint32_t *) 0,			/* 0x90 USB Low  Priority */
	(uint32_t *) 0,			/* 0x94 DAC */
	(uint32_t *) 0,			/* 0x98 COMP through EXTI Line */
	(uint32_t *) 0,			/* 0x9c EXTI Line 9..5 */
	(uint32_t *) 0,			/* 0xa0 LCD */
	(uint32_t *) 0,			/* 0xa4 TIM9 */
	(uint32_t *) 0,			/* 0xa8 TIM10 */
	(uint32_t *) 0,			/* 0xac TIM11 */
	(uint32_t *) 0,			/* 0xb0 TIM2 */
	(uint32_t *) 0,			/* 0xb4 TIM3 */
	(uint32_t *) tim4_irq_handler,	/* 0xb8 TIM4 */
};
