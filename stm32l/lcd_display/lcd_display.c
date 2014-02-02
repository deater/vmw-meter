/* Write something to the LCD Display */

#include <stdint.h>

#include "stm32l.h"

#define STACK_TOP 0x20000800

#define SEGA	0x8000
#define SEGB	0x4000
#define SEGC	0x2000
#define SEGD	0x1000
#define SEGE	0x0800
#define SEGF	0x0400
#define SEGG	0x0200
#define SEGH	0x0100
#define SEGJ	0x0080
#define SEGK	0x0040
#define SEGM	0x0020
#define SEGN	0x0010
#define SEGP	0x0008
#define SEGQ	0x0004
#define SEGCOLON	0x0002
#define SEGBAR		0x0002
#define SEGDP		0x0001

static short font[128]={
	0,
	0,
	0,
	0,
	0,
	0,	/* 5 */
	0,
	0,
	0,
	0,
	0,	/* 10 */
	0,
	0,
	0,
	0,
	0,	/* 15 */
	0,
	0,
	0,
	0,
	0,	/* 20 */
	0,
	0,
	0,
	0,
	0,	/* 25 */
	0,
	0,
	0,
	0,
	0,	/* 30 */
	0,
	0,	/* ' '	32 */
	0,	/* !	33 */
	0,      /* "	34 */
	0,	/* #	35 */
	0,	/* $	36 */
	0,	/* %	37 */
	0,	/* &	38 */
	0,	/* '	39 */
	0,	/* (	40 */
	0,	/* )	41 */
	0,	/* *	42 */
	0,	/* +	43 */
	0,	/* ,	44 */
	0,	/* -	45 */
	0,	/* .	46 */
	0,	/* /	47 */
	0,	/* 0	48 */
	0,	/* 1	49 */
	0,	/* 2	50 */
	0,	/* 3	51 */
	0,	/* 4	52 */
	0,	/* 5	53 */
	0,	/* 6	54 */
	0,	/* 7	55 */
	0,	/* 8	56 */
	0,	/* 9	57 */
	0,	/* :	58 */
	0,	/* ;	59 */
	0,	/* <	60 */
	0,	/* =	61 */
	0,	/* >	62 */
	0,	/* ?	63 */
	0,	/* @	64 */
	SEGB|SEGC|SEGK|SEGM|SEGQ,	/* A	65 */
	0,	/* B	66 */
	0,	/* C	67 */
	0,	/* D	68 */
	SEGA|SEGD|SEGE|SEGF|SEGG,	/* E	69 */
};



static void delay(int length) {

	volatile int i;

	for(i=0;i<length;i++) asm("nop");

}


GPIO_TypeDef *gpioa=(GPIO_TypeDef *)GPIOA_BASE;
GPIO_TypeDef *gpiob=(GPIO_TypeDef *)GPIOB_BASE;
GPIO_TypeDef *gpioc=(GPIO_TypeDef *)GPIOC_BASE;
RCC_TypeDef *rcc=(RCC_TypeDef *)RCC_BASE;
PWR_TypeDef *pwr=(PWR_TypeDef *)PWR_BASE;
RTC_TypeDef *rtc=(RTC_TypeDef *)RTC_BASE;
LCD_TypeDef *lcd=(LCD_TypeDef *)LCD_BASE;

static void lcd_clock_init(void) {

	/* enable power to clock interface */
	rcc->APB1ENR |= RCC_APB1ENR_PWREN;

	/* Disable write protection */
	pwr->CR	|= PWR_CR_DBP;

	/* Use LSI as RTC clock */
	rcc->CSR |= RCC_CSR_RTCSEL_LSI;

	/* enable RTC clock */
	rcc->CSR |= RCC_CSR_RTCEN;

	/* Disable write-protection for RTC registers */
	rtc->WPR = 0xca;
	rtc->WPR = 0x53;

	/* Wait for MSI clock to be read */
	while( (rcc->CR & RCC_CR_MSIRDY) == 0 ) ;

	/* enable LCD clock */
	rcc->APB1ENR |= RCC_APB1ENR_LCDEN;

	/* enable SYSCFG clock */
	rcc->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

	/* enable LSI (internal low-speed oscilator) and wait until ready */
	rcc->CSR |= RCC_CSR_LSION;
	while ( (rcc->CSR & RCC_CSR_LSIRDY) == 0 );

	/* Select LSI as LCD Clock Source */
	rcc->CSR &= ~RCC_CSR_RTCSEL_LSI;
	rcc->CSR |= RCC_CSR_RTCSEL_LSI;

	/* Enable RTC */
	rcc->CSR |= RCC_CSR_RTCEN;
}

void lcd_pin_init(void) {

	/* Enable GPIOA and GPIOB and GPIOC clocks*/
	rcc->AHBENR |= (AHBENR_GPIOAEN|AHBENR_GPIOBEN|AHBENR_GPIOCEN);

	/* GPIOA mode alterfunc (0b10) 15,10,9,8,3,2,1 */
#define GPIOA_MODE_MASK	 0xc03f00fc
#define GPIOA_MODE_ALTER 0x802a00a8
	gpioa->MODER&=~GPIOA_MODE_MASK;
	gpioa->MODER|=GPIOA_MODE_ALTER;
	/* output mode?  output speed? pull-up/down? */

	/* Port A, pins 15,10,9,8,3,2,1 */
	/* Alternate function mode 0xb (LCD) */
			/*76543210*/
#define LCD_A_L_MASK	0x0000fff0
#define LCD_A_L_ENABLE	0x0000bbb0
			/*fedcba98*/
#define	LCD_A_H_MASK	0xf0000fff
#define LCD_A_H_ENABLE	0xb0000bbb

	gpioa->AFR[0]&=~LCD_A_L_MASK;
	gpioa->AFR[0]|=LCD_A_L_ENABLE;

	gpioa->AFR[1]&=~LCD_A_H_MASK;
	gpioa->AFR[1]|=LCD_A_H_ENABLE;


	/* GPIOB mode alterfunc (0b10) 15,14,13,12,11,10,9,8,5,4,3 */
#define GPIOB_MODE_MASK	 0xffff0fc0
#define GPIOB_MODE_ALTER 0xaaaa0a80
	gpiob->MODER&=~GPIOB_MODE_MASK;
	gpiob->MODER|=GPIOB_MODE_ALTER;

	/* Port B, pins 15,14,13,12,11,10,9,8,5,4,3 */
	/* Alternate function mode 0xb (LCD) */
			/*76543210*/
#define LCD_B_L_MASK	0x00fff000
#define LCD_B_L_ENABLE	0x00bbb000
			/*fedcba98*/
#define	LCD_B_H_MASK	0xffffffff
#define LCD_B_H_ENABLE	0xbbbbbbbb

	gpiob->AFR[0]&=~LCD_B_L_MASK;
	gpiob->AFR[0]|=LCD_B_L_ENABLE;

	gpiob->AFR[1]&=~LCD_B_H_MASK;
	gpiob->AFR[1]|=LCD_B_H_ENABLE;


	/* GPIOC mode alterfunc (0b10) 11,10,9,8,7,6,3,2,1,0 */
#define GPIOC_MODE_MASK	 0x00fff0ff
#define GPIOC_MODE_ALTER 0x00aaa0aa
	gpioc->MODER&=~GPIOC_MODE_MASK;
	gpioc->MODER|=GPIOC_MODE_ALTER;

	/* Port C, pins 11,10,9,8,7,6,3,2,1,0 */
	/* Alternate function mode 0xb (LCD) */
			/*76543210*/
#define LCD_C_L_MASK	0xff00ffff
#define LCD_C_L_ENABLE	0xbb00bbbb
			/*fedcba98*/
#define	LCD_C_H_MASK	0x0000ffff
#define LCD_C_H_ENABLE	0x0000bbbb

	gpioc->AFR[0]&=~LCD_C_L_MASK;
	gpioc->AFR[0]|=LCD_C_L_ENABLE;

	gpioc->AFR[1]&=~LCD_C_H_MASK;
	gpioc->AFR[1]|=LCD_C_H_ENABLE;

}

void lcd_config(void) {

	/************/
	/* Setup CR */
	/************/

	/* Set Bias to 1/3 */
	lcd->CR&=~LCD_CR_BIAS_MASK;
	lcd->CR|=LCD_CR_BIAS_1_3;

	/* Set Duty to 1/4 */
	lcd->CR&=~LCD_CR_DUTY_MASK;
	lcd->CR|=LCD_CR_DUTY_1_4;

	/* Set enable mux segment */
	lcd->CR|=LCD_CR_MUX_SEG;

	/* select internal voltage */
	lcd->CR&=~LCD_CR_VSEL;

	/*************/
	/* Setup FCR */
	/*************/

	lcd->FCR = 0;	// reset

	/* See Table 60 in the manual */
	/* set to 30.12Hz: ps=4 div=1 duty = 1/4 */

	/* Set prescaler to 4 */
	lcd->FCR &= ~LCD_FCR_PS_MASK;
	lcd->FCR |= (4<<22);

	/* Set clock divider to 1 */
	lcd->FCR &=  ~LCD_FCR_DIV_MASK;
	lcd->FCR |= (1<<18);

	/* contrast = mean */
	lcd->FCR &= ~LCD_FCR_CC_MASK;
	lcd->FCR |= LCD_FCR_CC_LCD4;

	/* Pulse width.  Longer takes more energy */
	//lcd->FCR &= LCD_FCR_PON_MASK;
	lcd->FCR |= (7<<4) ;

	/* wait until FCRSF of LCD_SR set */
	while( !(lcd->SR & LCD_SR_FCRSF));

	/* set LCDEN */
	lcd->CR |= LCD_CR_LCDEN;

	/* wait for LCDEN in LCD_SR */
	while( !(lcd->SR & LCD_SR_ENS));

	/* wait for booster for RDY in LCD_SR */
	while( !(lcd->SR & LCD_SR_RDY));
}

void lcd_display(unsigned int *buffer) {

	int i;

	/* wait until protection gone */
	while((lcd->SR & LCD_SR_UDR)) ;

	for(i=0;i<16;i++) {
		lcd->RAM[i]=buffer[i];
	}

	/* request update of display */
	lcd->SR |= LCD_SR_UDR;

}

static unsigned char lookup1[6]={28,26,24,20,18,17};
static unsigned char lookup2[6]={1,7,9,11,13,15};
static unsigned char lookup3[6]={0,2,8,10,12,14};
static unsigned char lookup4[6]={29,27,25,21,19,16};

void lcd_convert(char *string, unsigned int *buffer) {

	int i;

	for(i=0;i<16;i++) buffer[i]=0;

	for(i=0;i<6;i++) {
		if (string[i]&SEGA) buffer[1*2]|=1<<lookup1[i];
		if (string[i]&SEGB) buffer[0*2]|=1<<lookup1[i];
		if (string[i]&SEGC) buffer[1*2]|=1<<lookup2[i];
		if (string[i]&SEGD) buffer[1*2]|=1<<lookup3[i];
		if (string[i]&SEGE) buffer[0*2]|=1<<lookup3[i];
		if (string[i]&SEGF) buffer[1*2]|=1<<lookup4[i];
		if (string[i]&SEGG) buffer[0*2]|=1<<lookup4[i];
		if (string[i]&SEGH) buffer[3*2]|=1<<lookup4[i];
		if (string[i]&SEGJ) buffer[3*2]|=1<<lookup1[i];
		if (string[i]&SEGK) buffer[2*2]|=1<<lookup1[i];
		if (string[i]&SEGM) buffer[0*2]|=1<<lookup2[i];
		if (string[i]&SEGN) buffer[3*2]|=1<<lookup3[i];
		if (string[i]&SEGP) buffer[2*2]|=1<<lookup3[i];
		if (string[i]&SEGQ) buffer[2*2]|=1<<lookup4[i];
		if (string[i]&SEGCOLON) buffer[1*2]|=1<<lookup3[i];
		if (string[i]&SEGDP) buffer[3*2]|=1<<lookup2[i];

	}
}

int main(void) {

	unsigned int lcd_buffer[16];

	lcd_clock_init();

	lcd_pin_init();

	lcd_config();

	lcd_convert("WEAVER",lcd_buffer);

#if 0
	lcd_buffer[0]=0x190bd605;
	lcd_buffer[2]=0x2c2f1206;
	lcd_buffer[4]=0x23300000;
	lcd_buffer[6]=0x00004001;
#endif

	lcd_display(lcd_buffer);


	/* busy wait forever */
	for(;;) {

	}

}

static void nmi_handler(void) {
	for(;;);
}

static void hardfault_handler(void) {
	for(;;);
}

extern unsigned long _etext,_data,_edata,_bss_start,_bss_end;

	/* Copy DATA and BSS segments into RAM */
void c_startup(void)	{

	unsigned long *src, *dst;

	/* Copy data segment */
	/* Using linker symbols */
	src = &_etext;
	dst = &_data;
	while(dst < &_edata) *(dst++) = *(src++);

	/* Zero out the BSS */
	src = &_bss_start;
	while(src < &_bss_end) *(src++) = 0;

	/* Call main() */
	main();

}


/* Vector Table */
unsigned int *myvectors[4]
__attribute__ ((section("vectors"))) = {
	(uint32_t *) STACK_TOP,		/* stack pointer      */
	(uint32_t *) c_startup,		/* code entry point   */
	(uint32_t *) nmi_handler,	/* NMI handler        */
	(uint32_t *) hardfault_handler	/* hard fault handler */
};
