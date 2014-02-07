/* Ultrasonic Range Finder */

#include <stdint.h>
#include "stm32l.h"

#include "delay_lib.h"
#include "lcd_lib.h"
#include "num_to_string.h"


int main(void) {

	unsigned int lcd_buffer[16];
//	char string[7],number[7];
//	unsigned int count=0,feet,inches,length;

	GPIO_TypeDef *gpiob=(GPIO_TypeDef *)GPIOB_BASE;
	RCC_TypeDef *rcc=(RCC_TypeDef *)RCC_BASE;
	TIM_TypeDef *tim4=(TIM_TypeDef *)TIM4_BASE;
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

	/* Set TIM4 for pins PB6 and PB7 */
	temp=gpiob->MODER;
	temp&=~0x0000f000;		/* clear values for pin 6 */
	temp|= (MODER_ALT<<(6*2));	/* Set output type to ALTERNATE */
	temp|= (MODER_ALT<<(7*2));	/* Set output type to ALTERNATE */
	gpiob->MODER = temp; 		/* Set Output mode */

	gpiob->AFR[0] |= AFR_TIM345<<(4*6);
					/* Pin B6 = AF2 (TIM4) */
	gpiob->AFR[0] |= AFR_TIM345<<(4*7);
					/* Pin B7 = AF2 (TIM4) */
					/* i.e. hook up to TIM4 clock */


	/* Clock Speed */
	temp=gpiob->OSPEEDR&~0x0000f000;
	temp|= 0x0000f000;		/* 40MHz clock speed */
	gpiob->OSPEEDR=temp;

	/* Pull-Up/Pull-Down */
	gpiob->PUPDR &=~0x0000f000;	/* No pull-up pull-down */

	/* Set output type */
//	gpiob->OTYPER &=~0x20;		/* Push-Pull */


	/******************/
	/* Set up Timer 4 */
	/******************/


	/* Enable Clock */
	rcc->APB1ENR |= RCC_APB1ENR_TIM4EN;

	/* MSI Clock is 2.097MHz */

	/* Want PWM mode */
	/* Cycle time set by ARR */
	/* Duty cycle in CCR1    */

	/* Divide by 2 to get 1.0485MHz */
	/* 0.953us			*/
	tim4->PSC = (2)-1;		/* Pre-scalar value */

	tim4->CCR1 = 10;		/* Load 5 into capture/compare 1 */
					/* */

	tim4->ARR = 20;			/* auto-reload register */
					/* causes overflow after 11 cycles */
					/* 10.5us */


	tim4->CR1 |= TIM_CR1_OPM;	/* One-pulse mode */
					/* Stop after hitting ARR first time */

	tim4->CCMR1 = 7<<4;		/* Use PWM mode 2 */
					/* chan1 low when CNT < CCR1 */
					/* chan1 high when CNT > CCR1  */

	tim4->CCER = TIM_CCER_CC1E;	/* Enable channel 1 output */

	tim4->CR1 |= TIM_CR1_CEN;	/* Enable timer */

	/* Loop forever */

	for(;;) {

		lcd_convert("WEAVER",lcd_buffer);
                lcd_display(lcd_buffer);

		busy_delay(50000);

		tim4->CR1 |= TIM_CR1_CEN;	/* Enable timer */

	}
}

static void nmi_handler(void) {
	for(;;);
}

static void hardfault_handler(void) {
	for(;;);
}


/* Vector Table */

#define STACK_TOP 0x20000800

unsigned int *myvectors[4]
__attribute__ ((section("vectors"))) = {
	(uint32_t *) STACK_TOP,		/* stack pointer      */
	(uint32_t *) main,		/* code entry point   */
	(uint32_t *) nmi_handler,	/* NMI handler        */
	(uint32_t *) hardfault_handler	/* hard fault handler */
};
