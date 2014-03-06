@ Blink the blue and green LEDs

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

	/* Enable GPIOB */

	/* Enable GPIOB clock */
	ldr	r2,[r1,#AHBENR]		@ rcc->AHBENR
	orr	r2,r2,#AHBENR_GPIOBEN
	str	r2,[r1,#AHBENR]


	/*******************/
	/* Configure GPIOB */
	/*******************/

	/* Set pin mode */

	ldr	r2,[r0,#MODER]		/* gpiob->MODER */
	and	r2,r2,#~0x0000f000	/* clear values for pins 6 and 7 */
	orr	r2, #( (MODER_OUTPUT<<(6*2)) | (MODER_OUTPUT<<(7*2)) )
					/* Set output for pins 6 and 7 */
	str	r2,[r0,#MODER]		/* Set Output mode */


	/* Set output type */
	ldrh	r2,[r0,#OTYPER]		/* gpiob->OTYPER */
	and	r2,#~0x60		/* Push-Pull */
	strh	r2,[r0,#OTYPER]

	/* Clock Speed */
	ldr	r2,[r0,#OSPEEDR]	/* gpiob->OSPEEDR */
	and	r2,#~0x0000f000		/* clear bits */
	orr	r2,#0x00005000		/* 2MHz clock speed */
	str	r2,[r0,#OSPEEDR]

	/* Pull-Up/Pull-Down */
	ldr	r2,[r0,#PUPDR]		/* gpiob->PUPDR */
	and	r2,#~0x0000f000		/* No pull-up pull-down */
	str	r2,[r0,#PUPDR]

blink_loop:

	/* Green off, Blue On */
	mov	r2,#(1<<7)		/* Set PB7 low  */
	strh	r2,[r0,#BSRRL]		/* gpiob->BSRRL */

	mov	r2,#(1<<6)		/* Set PB6 high */
	strh	r2,[r0,#BSRRH]		/* gpiob->BSRRH */

	mov	r3,#65536
	bl	busy_delay

	/* Green on, Blue Off */
	mov	r2,#(1<<7)		/* Set PB7 high */
	strh	r2,[r0,BSRRH]		/* gpiob->BSRRH */

	mov	r2,#(1<<6)		/* Set PB6 low  */
	strh	r2,[r0,BSRRL]		/* gpiob->BSRRL */

	mov	r3,#65536
	bl	busy_delay

	b	blink_loop

.thumb_func
loop_forever:
	b	loop_forever

busy_delay:
	cmp	r3,#0
	beq	done_delay

	sub	r3,r3,#1

	b	busy_delay
done_delay:
	blx	lr



