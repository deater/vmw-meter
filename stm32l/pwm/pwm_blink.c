/* Blink the blue LED using PWM */

#include <stdint.h>
#include "stm32l.h"


int main(void) {

	GPIO_TypeDef *gpiob=(GPIO_TypeDef *)GPIOB_BASE;
	RCC_TypeDef *rcc=(RCC_TypeDef *)RCC_BASE;
	TIM_TypeDef *tim4=(TIM_TypeDef *)TIM4_BASE;
	uint32_t temp;

	/* Enable GPIOB */

	rcc->AHBENR |= AHBENR_GPIOBEN;	/* Enable GPIOB clock */

	/*******************/
	/* Configure GPIOB */
	/*******************/

	/* Set pin mode */
	temp=gpiob->MODER;
	temp&=~0x00003000;		/* clear values for pin 6 */
	temp|= (MODER_ALT<<(6*2));	/* Set output type to ALTERNATE */
	gpiob->MODER = temp; 		/* Set Output mode */

	gpiob->AFR[0] |= AFR_TIM345<<(4*6);
					/* Pin B6 = AF2 (TIM4) */
					/* i.e. hook up to TIM4 clock */


	/* Clock Speed */
	temp=gpiob->OSPEEDR&~0x00003000;
	temp|= 0x00003000;	/* 40MHz clock speed */
	gpiob->OSPEEDR=temp;

	/* Pull-Up/Pull-Down */
	gpiob->PUPDR &=~0x00003000;	/* No pull-up pull-down */

	/* Set output type */
	gpiob->OTYPER &=~0x20;		/* Push-Pull */


	/* Enable Clock */
	rcc->APB1ENR |= RCC_APB1ENR_TIM4EN;

	/* Set up Timer 4 */

	tim4->PSC = (2097000/1000)-1;	/* Pre-scalar value */
	tim4->ARR = 1000;		/* Auto re-load value */


	tim4->CCMR1 = TIM_CCMR1_OC1M_TOGGLE;
					/* Toggle state */
	tim4->CCER = TIM_CCER_CC1E;	/* Enable Capture/Compare Output 1 */
	tim4->CR1 = TIM_CR1_CEN;	/* Enable timer */

	/* Loop forever */
	for(;;);
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
