@ Dimm the LEDs using PWM

.include "../include/stm32l_asm.h"

.cpu cortex-m3
.syntax unified
.thumb


.word	0x20008000	/* top of stack address */
.word	_start		/* reset vector*/
.word	loop_forever	/* nmi vector */
.word	loop_forever	/* hard fault vector */
			/* There are more vectors, assume won't happen? */

.globl _start
.thumb_func
_start:

	ldr	r0,=GPIOB_BASE
	ldr	r1,=RCC_BASE

	/****************/
	/* Enable GPIOB */
	/****************/

	ldr	r2,[r1,#AHBENR]		@ rcc->AHBENR
	orr	r2,r2,#AHBENR_GPIOBEN
	str	r2,[r1,#AHBENR]


	/*******************/
	/* Configure GPIOB */
	/*******************/

	/* Set pin mode */

	ldr	r2,[r0,#MODER]		/* gpiob->MODER */
	and	r2,r2,#~0x0000f000	/* clear values for pins 6 and 7 */
	orr	r2, #( (MODER_ALT<<(6*2)) | (MODER_ALT<<(7*2)) )
					/* Set alternate function */
					/* for pins 6 and 7 */
	str	r2,[r0,#MODER]		/* Set Output mode */

	ldr	r2,[r0,#AFR]		/* gpiob->AFR[0] */
	and	r2,#~0xff000000		/* clear alternate function */
	orr	r2,#( (AFR_TIM345<<28) | (AFR_TIM345<<24))
					/* Enable TIM4 on both */
	str	r2,[r0,#AFR]

	/* Set output type */
	ldrh	r2,[r0,#OTYPER]		/* gpiob->OTYPER */
	and	r2,#~0x60		/* Push-Pull */
	strh	r2,[r0,#OTYPER]

	/* Clock Speed */
	ldr	r2,[r0,#OSPEEDR]	/* gpiob->OSPEEDR */
	and	r2,#~0x0000f000		/* clear bits */
	orr	r2,#0x0000f000		/* 40MHz clock speed */
	str	r2,[r0,#OSPEEDR]

	/* Pull-Up/Pull-Down = Open Drain */
	ldr	r2,[r0,#PUPDR]		/* gpiob->PUPDR */
	and	r2,#~0x0000f000		/* No pull-up pull-down */
	str	r2,[r0,#PUPDR]


	/* Enable TIM4 Clock */
	ldr	r2,[r1,#APB1ENR]	/* rcc->APB1ENR */
	orr	r2,#RCC_APB1ENR_TIM4EN
	str	r2,[r1,#APB1ENR]

	/* Default clock: MSR Range 5, 2.097MHz */
	/* Set the frequency as 1kHz		*/
	ldr	r4,=TIM4_BASE

	mov	r2,#(2097000/1000-1)	/* prescalar value */
	str	r2,[r4,#PSC]		/* tim4->PSC */

	mov	r2,#1000
	str	r2,[r4,#ARR]		/* tim4->ARR */

	mov	r2,#500
	str	r2,[r4,#CCR1]		/* tim4->CCR1 */

	mov	r2,#(TIM_CCMR1_OC1M_TOGGLE)
	str	r2,[r4,#CCMR1]		/* tim4->CCMR1 */

	mov	r2,#TIM_CCER_CC1E
	str	r2,[r4,#CCER]		/* tim4->CCER */

	mov	r2,#TIM_CR1_CEN
	str	r2,[r4,CR1]		/* tim4->CR1 */

	/* Blinks on 1s, off 1s */
	/* starts out off, as CNT crosses CCR1 at 0.5, toggles on */
	/* as CNT hits ARR at 1s, resets to 0 */
	/* as CNT crosses CCR1 again at 1.5s, toggles off */

blink_loop:

loop_forever:
	b	loop_forever


