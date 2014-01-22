/* Blink the blue and green LEDs */

#include <stdint.h>

#include "stm32l.h"

#define STACK_TOP 0x20000800

static void delay(int length) {

	volatile int i;

	for(i=0;i<length;i++) asm("nop");

}

int main(void) {

	GPIO_TypeDef *gpiob=(GPIO_TypeDef *)GPIOB_BASE;
	RCC_TypeDef *rcc=(RCC_TypeDef *)RCC_BASE;
	uint32_t temp;

	/* Enable GPIOB */

	rcc->AHBENR |= AHBENR_GPIOBEN;	/* Enable GPIOB clock */

	/*******************/
	/* Configure GPIOB */
	/*******************/

	/* Set pin mode */
	temp=gpiob->MODER;
	temp&=~0x0000f000;		/* clear values for pins 6 and 7 */
	temp|= (MODER_OUTPUT<<(6*2)) | (MODER_OUTPUT<<(7*2));
					/* Set output for pins 6 and 7 */
	gpiob->MODER = temp; 		/* Set Output mode */


	/* Set output type */
	gpiob->OTYPER &=~0x60;		/* Push-Pull */


	/* Clock Speed */
	temp=gpiob->OSPEEDR&~0x0000f000;
	temp|= 0x00005000;	/* 2MHz clock speed */
	gpiob->OSPEEDR=temp;

	/* Pull-Up/Pull-Down */
	gpiob->PUPDR &=~0x0000f000;	/* No pull-up pull-down */

	//gpiob->IDR = input (read-only)
	//gpiob->ODR = output (read/write)

	//gpiob->ODR |= (1<<6) | (1<<7);

	/* Alternately you can set the BSRR registers */
	/* which allow one-shot setting/resetting of bits */

	for(;;) {
		/* Green on, Blue Off */
		gpiob->BSRRL |= 1<<7;
		gpiob->BSRRH |= 1<<6;

		delay(60000);

		/* Green off, Blue On */
		gpiob->BSRRH |= 1<<7;
		gpiob->BSRRL |= 1<<6;

		delay(60000);
	}

	for(;;);
}

static void nmi_handler(void) {
	for(;;);
}

static void hardfault_handler(void) {
	for(;;);
}


/* Vector Table */
unsigned int *myvectors[4]
__attribute__ ((section("vectors"))) = {
	(uint32_t *) STACK_TOP,		/* stack pointer      */
	(uint32_t *) main,		/* code entry point   */
	(uint32_t *) nmi_handler,	/* NMI handler        */
	(uint32_t *) hardfault_handler	/* hard fault handler */
};
