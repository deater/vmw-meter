@ Use SysTick and interrupts to blink LEDs

.include "../include/stm32l_asm.h"

.cpu cortex-m3
.syntax unified
.thumb

.word	0x20000800	/* 0x00 top of stack address */
.word	_start		/* 0x04 reset vector*/
.word	blargh		/* 0x08 nmi vector */
.word	hard_fault	/* 0x0c hard fault vector */
.word	0	/* 0x10 mem_manage */
.word	0	/* 0x14 bus_fault  */
.word	0	/* 0x18 usage_fault */
.word	0	/* 0x1c reserved */
.word	0	/* 0x20 reserved */
.word	0	/* 0x24 reserved */
.word	0	/* 0x28 reserved */
.word	0	/* 0x2c SVC */
.word	0	/* 0x30 debug */
.word	0	/* 0x34 reserved */
.word	0	/* 0x38 pendable */
.word	timer_tick	/* 0x3c timer tick */


.globl _start
.thumb_func
_start:

	/*********************/
	/* Copy Data Section */
	/*********************/

	ldr	r2,=_etext	/* src */
	ldr	r3,=_data	/* dest */
	ldr	r5,=_edata	/* end of data */
copy_data_loop:
	cmp	r3,r5
	beq	copy_data_end
	ldr	r4,[r2]
	str	r4,[r3],#4
copy_data_end:


	/*****************/
	/* Zero out BSS  */
	/*****************/
	ldr	r2,=_bss_start
	ldr	r3,=_bss_end
	mov	r4,#0

zero_bss_loop:
	cmp	r2,r3
	beq	zero_bss_end
	str	r4,[r2],#4
zero_bss_end:


	/************************/
	/* Set up global values */
	/************************/

	ldr	r0,=GPIOB_BASE
	ldr	r1,=RCC_BASE


	/****************/
	/* Enable GPIOB */
	/****************/
enable_gpiob:
	ldr	r2,[r1,#AHBENR]		/* rcc->AHBENR */
	orr	r2,r2,#AHBENR_GPIOBEN
	str	r2,[r1,#AHBENR]


	/*******************/
	/* Configure GPIOB */
	/*******************/

	/* Set pin mode */

	ldr	r2,[r0,#MODER]		/* gpiob->MODER */
	and	r2,r2,#~0x0000f000	/* clear values for pins 6 and 7 */
	orr	r2, #( (MODER_OUTPUT<<(6*2)) | (MODER_OUTPUT<<(7*2)) )
					/* Set output  mode */
					/* for pins 6 and 7 */
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

	/* Pull-Up/Pull-Down = Open Drain */
	ldr	r2,[r0,#PUPDR]		/* gpiob->PUPDR */
	and	r2,#~0x0000f000		/* No pull-up pull-down */
	str	r2,[r0,#PUPDR]


	/*********************/
	/* Configure SysTick */
	/*********************/
config_systick:
	ldr	r1,=SYSTICK_BASE

@	ldr	r2,[r1,#SYSTICK_CTRL]
@	bic	r2,r2,#SYSTICK_ENABLE		/* clear enable */
@	bic	r2,r2,#SYSTICK_TICKINT		/* clear interrupt */
@	bic	r2,r2,#SYSTICK_CLKSOURCE	/* clear source */


	/* specify ticks between interrupts */

	ldr	r2,=26200				/* why 262?*/
	str	r2,[r1,#SYSTICK_RELOAD]

	/* clear current val register */
	mov	r2,#0
	str	r2,[r1,#SYSTICK_VAL]


	/* Systick cannot be disabled! */
	/* so no need to enable it */


	/* You can set the priority in SHPR3 */

shpr:
@	ldr	r4,=0xe000ed20
@	ldr	r5,[r4]
@	ldr	r6,=(0x10<<24)
@	orr	r5,r5,r6
@	str	r5,[r4]

	/* enable IRQ and timer */
	ldr	r2,[r1,#SYSTICK_CTRL]
	orr	r2,r2,#(SYSTICK_ENABLE|SYSTICK_TICKINT|SYSTICK_CLKSOURCE)
	str	r2,[r1,#SYSTICK_CTRL]



@	ldr	r0,=GPIOB_BASE

@	mov     r2,#(1<<7)              /* Set PB7 low */
@	strh    r2,[r0,#BSRRL]           /* gpiob->BSRRH */

@	mov     r2,#(1<<6)              /* Set PB7 high */
@	strh    r2,[r0,#BSRRH]           /* gpiob->BSRRH */

.thumb_func
loop_forever:

	b	loop_forever


.thumb_func
blargh:

.thumb_func
hard_fault:
	/* Wasted a *long* time tracking down a bug triggering hard fault */
	/* It ended up being due to the stack value being set wrong */

	/* To decode a hard fault you can look at the HFSR (0xe000ed2c) */
	/* and then the BFSR (0xe000ed28) and if the right bits are set */
	/* also the BFAR (0xe000ed38)					*/

	/* If the stack is working (mine wasn't) you can read the save	*/
	/* values off the stack as well.				*/

	@ MRS R8, MSP

	@ mov	r2,#(1<<7)
	@ strh	r2,[r0,#BSRRH]		/* Blue LED on hard fault */

	@ mov	r2,#(1<<6)		/* Set PB7 high */
	@ strh	r2,[r0,#BSRRL]		/* gpiob->BSRRH */

	@ add	r7,r7,#1		/* incremement hard fault count */

	b	hard_fault		/* returning is probably bad */


.globl timer_tick
.thumb_func
timer_tick:

	add	r6,r6,#1

	ldr	r3,=led_state
	ldrb	r2,[r3]
	add	r2,r2,#1		/* count to see we are indeed	*/


	mov	r4,#0
	cmp	r2,#(1<<4)
	it	ge
	movge	r4,#1

	cmp	r2,#(1<<5)
	it	ge
	movge	r2,#0

	str	r2,[r3]

	cmp	r4,0

	ite	eq
	moveq	r2,#(1<<7)		/* Set PB7 low  */
	movne	r2,#(1<<6)		/* Set PB6 low  */
	strh	r2,[r0,BSRRL]		/* gpiob->BSRRL */

	ite	eq
	moveq	r2,#(1<<6)		/* Set PB6 high */
	movne	r2,#(1<<7)		/* Set PB7 high */
	strh	r2,[r0,#BSRRH]		/* gpiob->BSRRH */

	bx	lr			/* return */


.bss
	.lcomm	led_state,4

