/* Write something to the LCD Display */

#include <stdint.h>

#include "stm32l.h"

#include "lcd_lib.h"

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
	SEGJ|SEGB,      /* "	34 */
	0,	/* #	35 */
	0,	/* $	36 */
	0,	/* %	37 */
	0,	/* &	38 */
	SEGJ,						/* '	39 */
	SEGK|SEGN,					/* (	40 */
	SEGH|SEGQ,					/* )	41 */
	SEGJ|SEGK|SEGM|SEGN|SEGP|SEGQ|SEGG|SEGH,	/* *	42 */
	SEGJ|SEGM|SEGP|SEGG,				/* +	43 */
	SEGQ,						/* ,	44 */
	SEGG|SEGM,					/* -	45 */
	SEGP,						/* .	46 */
	SEGQ|SEGK,					/* /	47 */
	SEGA|SEGB|SEGC|SEGD|SEGE|SEGF|SEGK|SEGQ,	/* 0	48 */
	SEGB|SEGC|SEGK,					/* 1	49 */
	SEGA|SEGB|SEGM|SEGG|SEGE|SEGD,			/* 2	50 */
	SEGA|SEGB|SEGM|SEGC|SEGD|SEGG,			/* 3	51 */
	SEGF|SEGG|SEGM|SEGB|SEGC,			/* 4	52 */
	SEGA|SEGF|SEGG|SEGN|SEGD,			/* 5	53 */
	SEGA|SEGF|SEGE|SEGD|SEGC|SEGG|SEGM,		/* 6	54 */
	SEGA|SEGK|SEGP|SEGG|SEGM,			/* 7	55 */
	SEGA|SEGB|SEGC|SEGD|SEGE|SEGF|SEGG|SEGM,	/* 8	56 */
	SEGF|SEGA|SEGB|SEGC|SEGG|SEGM,			/* 9	57 */
	0,	/* :	58 */
	0,	/* ;	59 */
	0,	/* <	60 */
	0,	/* =	61 */
	0,	/* >	62 */
	0,	/* ?	63 */
	0,	/* @	64 */
	SEGB|SEGC|SEGK|SEGM|SEGQ,		/* A	65 */
	SEGA|SEGD|SEGE|SEGF|SEGG|SEGK|SEGN,	/* B	66 */
	SEGA|SEGD|SEGE|SEGF,			/* C	67 */
	SEGA|SEGB|SEGC|SEGD|SEGJ|SEGP,		/* D	68 */
	SEGA|SEGD|SEGE|SEGF|SEGG,		/* E	69 */
	SEGA|SEGE|SEGF|SEGG,			/* F	70 */
	SEGA|SEGC|SEGD|SEGE|SEGF|SEGM,		/* G	71 */
	SEGB|SEGC|SEGE|SEGF|SEGG|SEGM,		/* H	72 */
	SEGA|SEGD|SEGJ|SEGP,			/* I	73 */
	SEGB|SEGC|SEGD|SEGE,			/* J	74 */
	SEGF|SEGE|SEGG|SEGK|SEGN,		/* K	75 */
	SEGD|SEGE|SEGF,				/* L	76 */
	SEGB|SEGC|SEGE|SEGF|SEGH|SEGK,		/* M	77 */
	SEGE|SEGF|SEGB|SEGC|SEGH|SEGN,		/* N	78 */
	SEGA|SEGB|SEGC|SEGD|SEGE|SEGF,		/* O	79 */
	SEGA|SEGB|SEGE|SEGF|SEGG|SEGM,		/* P	80 */
	SEGA|SEGB|SEGC|SEGD|SEGE|SEGF|SEGQ,	/* Q	81 */
	SEGA|SEGB|SEGE|SEGF|SEGG|SEGM|SEGN,	/* R	82 */
	SEGA|SEGF|SEGG|SEGM|SEGC|SEGD,		/* S	83 */
	SEGA|SEGJ|SEGP,				/* T	84 */
	SEGB|SEGC|SEGD|SEGE|SEGF,		/* U	85 */
	SEGK|SEGQ|SEGE|SEGF,			/* V	86 */
	SEGF|SEGE|SEGQ|SEGN|SEGC|SEGB,		/* W	87 */
	SEGH|SEGK|SEGN|SEGQ,			/* X	88 */
	SEGH|SEGK|SEGP,				/* Y	89 */
	SEGA|SEGK|SEGQ|SEGD,			/* Z	90 */
};

static GPIO_TypeDef *gpioa=(GPIO_TypeDef *)GPIOA_BASE;
static GPIO_TypeDef *gpiob=(GPIO_TypeDef *)GPIOB_BASE;
static GPIO_TypeDef *gpioc=(GPIO_TypeDef *)GPIOC_BASE;
static RCC_TypeDef *rcc=(RCC_TypeDef *)RCC_BASE;
static PWR_TypeDef *pwr=(PWR_TypeDef *)PWR_BASE;
static RTC_TypeDef *rtc=(RTC_TypeDef *)RTC_BASE;
static LCD_TypeDef *lcd=(LCD_TypeDef *)LCD_BASE;

void lcd_clock_init(void) {

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

	int i,which,dp;

	for(i=0;i<16;i++) buffer[i]=0;

	for(i=0;i<6;i++) {
		dp=string[i]&0x80;
		which=((int)string[i])&0x7f;
		if (font[which]&SEGA) buffer[1*2]|=1<<lookup1[i];
		if (font[which]&SEGB) buffer[0*2]|=1<<lookup1[i];
		if (font[which]&SEGC) buffer[1*2]|=1<<lookup2[i];
		if (font[which]&SEGD) buffer[1*2]|=1<<lookup3[i];
		if (font[which]&SEGE) buffer[0*2]|=1<<lookup3[i];
		if (font[which]&SEGF) buffer[1*2]|=1<<lookup4[i];
		if (font[which]&SEGG) buffer[0*2]|=1<<lookup4[i];
		if (font[which]&SEGH) buffer[3*2]|=1<<lookup4[i];
		if (font[which]&SEGJ) buffer[3*2]|=1<<lookup1[i];
		if (font[which]&SEGK) buffer[2*2]|=1<<lookup1[i];
		if (font[which]&SEGM) buffer[0*2]|=1<<lookup2[i];
		if (font[which]&SEGN) buffer[3*2]|=1<<lookup3[i];
		if (font[which]&SEGP) buffer[2*2]|=1<<lookup3[i];
		if (font[which]&SEGQ) buffer[2*2]|=1<<lookup4[i];
		if (font[which]&SEGCOLON) buffer[1*2]|=1<<lookup3[i];
		if (font[which]&SEGDP) buffer[3*2]|=1<<lookup2[i];
		if (dp) buffer[3*2]|=1<<lookup2[i];

	}
}

