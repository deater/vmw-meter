@ Blink the blue and green LEDs

.include "../include/stm32l_asm.h"

.cpu cortex-m3
.thumb

.word	0x20008000	/* top of stack address */
.word	_start		/* 1 Reset */
.word	loop_forever	/* 2 NMI */
.word	loop_forever	/* 3 HardFault */
.word	loop_forever	/* 4 MemManage */


_start:	nop

loop_forever:
	b	loop_forever



@int main(void) {
@
@	GPIO_TypeDef *gpiob=(GPIO_TypeDef *)GPIOB_BASE;
@	RCC_TypeDef *rcc=(RCC_TypeDef *)RCC_BASE;
@	uint32_t temp;

@	/* Enable GPIOB */

@	rcc->AHBENR |= AHBENR_GPIOBEN;	/* Enable GPIOB clock */

@	/*******************/
@	/* Configure GPIOB */
@	/*******************/

@	/* Set pin mode */
@	temp=gpiob->MODER;
@	temp&=~0x0000f000;		/* clear values for pins 6 and 7 */
@	temp|= (MODER_OUTPUT<<(6*2)) | (MODER_OUTPUT<<(7*2));
					/* Set output for pins 6 and 7 */
@	gpiob->MODER = temp; 		/* Set Output mode */


@	/* Set output type */
@	gpiob->OTYPER &=~0x60;		/* Push-Pull */


@	/* Clock Speed */
@	temp=gpiob->OSPEEDR&~0x0000f000;
@	temp|= 0x00005000;	/* 2MHz clock speed */
@	gpiob->OSPEEDR=temp;

@	/* Pull-Up/Pull-Down */
@	gpiob->PUPDR &=~0x0000f000;	/* No pull-up pull-down */

@	//gpiob->IDR = input (read-only)
@	//gpiob->ODR = output (read/write)

@	//gpiob->ODR |= (1<<6) | (1<<7);

@	/* Alternately you can set the BSRR registers */
@	/* which allow one-shot setting/resetting of bits */

@	for(;;) {
@		/* Green on, Blue Off */
@		gpiob->BSRRL |= 1<<7;
@		gpiob->BSRRH |= 1<<6;

@		busy_delay(60000);

@		/* Green off, Blue On */
@		gpiob->BSRRH |= 1<<7;
@		gpiob->BSRRL |= 1<<6;

@		busy_delay(60000);
@	}

@	for(;;);
@}
