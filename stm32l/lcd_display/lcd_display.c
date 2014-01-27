/* Write something to the LCD Display */

#include <stdint.h>

#include "stm32l.h"

#define STACK_TOP 0x20000800

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
	lcd->FCR &=  ~LCD_FCR_DIV_MASK
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


int main(void) {

	lcd_clock_init();

	lcd_pin_init();

	lcd_config();

	/* wait until protection gone */
	while((lcd->SR & LCD_SR_UDR)) ;

	/* Display WEAVER */
	lcd->RAM[0]=0x190bd605;
	lcd->RAM[2]=0x2c2f1206;
	lcd->RAM[4]=0x23300000;
	lcd->RAM[6]=0x00004001;

	/* request update of display */
	lcd->SR |= LCD_SR_UDR;

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
