/* Start/stop the LEDs with the pushbutton */

#include <stdint.h>

#include "stm32l.h"

#define STACK_TOP 0x20000800

static void delay(int length) {

	volatile int i;

	for(i=0;i<length;i++) asm("nop");

}

int main(void) {

	uint32_t blinking=1,keydown=0,green=0;

	GPIO_TypeDef *gpioa=(GPIO_TypeDef *)GPIOA_BASE;
	GPIO_TypeDef *gpiob=(GPIO_TypeDef *)GPIOB_BASE;
	RCC_TypeDef *rcc=(RCC_TypeDef *)RCC_BASE;
	uint32_t temp;

	/* Enable GPIOA and GPIOB clocks*/
	rcc->AHBENR |= (AHBENR_GPIOAEN|AHBENR_GPIOBEN);

	/*******************/
	/* Configure GPIOA */
	/*******************/

	/* Set pin mode */
	temp=gpioa->MODER;
	temp&=~0x00000003;		/* Clear values for pin 0 */
	temp|= MODER_INPUT<<0;		/* Set input for pin 0 */
	gpioa->MODER = temp;		/* Set Output mode */

	/* Set output type */
	gpioa->OTYPER &=~0x1;		/* Push-Pull */

	/* Clock Speed */
	temp=gpioa->OSPEEDR&~0x00000003;
	temp|= 0x00000001;	/* 2MHz clock speed */
	gpioa->OSPEEDR=temp;

	/* Pull-Up/Pull-Down */
	gpiob->PUPDR &=~0x00000003;	/* No pull-up pull-down */

	//gpiob->IDR = input (read-only)


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

	/* set both LEDs to off */

	gpiob->BSRRH |= 3<<6;

	for(;;) {

		if (gpioa->IDR&1) {
			if (keydown==0) {
				blinking=!blinking;
				keydown=1;
			}
		} else {
			keydown=0;
		}


		if (blinking) {
			if (green) {
				/* Green on, Blue Off */
				gpiob->BSRRL |= 1<<7;
				gpiob->BSRRH |= 1<<6;

				delay(60000);
				green=0;
			}

			else {

				/* Green off, Blue On */
				gpiob->BSRRH |= 1<<7;
				gpiob->BSRRL |= 1<<6;

				delay(60000);
				green=1;
			}
		}
		else {

		}

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
