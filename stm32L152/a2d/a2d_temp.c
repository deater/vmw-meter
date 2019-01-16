/* Analog to Digital Conversion */

/* Assumes you have  TMP36 analog temperature sensor hooked up */
/* to VDD/PC0/GND */

#include <stdint.h>
#include "stm32l.h"

#include "delay_lib.h"
#include "lcd_lib.h"
#include "num_to_string.h"


ADC_TypeDef *adc;
static int result=0;

static char string[10];

static void convert_to_temp(int voltage, int type) {

	int temp,tenthousands,thousands,hundreds,tens,ones;

	temp=voltage;

	if (type==0) { /* voltage */


		string[3]='V';

		hundreds=temp/100;
		string[0]=hundreds+'0';

		temp-=hundreds*100;

		tens=temp/10;

		string[1]=tens+'0';

		ones=temp%10;

		string[2]=ones+'0';

		return;
	}

	tenthousands=temp/10000;
	string[0]=tenthousands+'0';

	temp-=tenthousands*10000;

	thousands=temp/1000;
	string[1]=thousands+'0';

	temp-=thousands*1000;

	hundreds=temp/100;
	string[2]=hundreds+'0';

	if (type==1) {
		string[3]='C';
	}

	if (type==2) {
		string[3]='F';
	}


	string[1]|=0x80;	/* set decimal point */

}

static void convert_result(int value) {

	int temp,thousands,hundreds,tens,ones;

	temp=value;

	thousands=temp/1000;
	string[0]=thousands+'0';

	temp-=thousands*1000;

	hundreds=temp/100;
	string[1]=hundreds+'0';

	temp-=hundreds*100;

	tens=temp/10;

	string[2]=tens+'0';

	ones=temp%10;

	string[3]=ones+'0';

}


int main(void) {

	unsigned int lcd_buffer[16];

	GPIO_TypeDef *gpioc=(GPIO_TypeDef *)GPIOC_BASE;
	RCC_TypeDef *rcc=(RCC_TypeDef *)RCC_BASE;
	adc=(ADC_TypeDef *)ADC_BASE;
	NVIC_ISER_TypeDef *nvic=(NVIC_ISER_TypeDef *)NVIC_ISER_BASE;
	uint32_t temp;

	/********************/
	/* Enable HSI clock */
	/********************/

	rcc->CR |= RCC_CR_HSION;

	/* Wait for HSI to be ready */
	while(!(rcc->CR&RCC_CR_HSIRDY)) ;

	/******************/
	/* configure LCD  */
	/******************/

	lcd_clock_init();

	lcd_pin_init();

	lcd_config();


	/*******************/
	/* Configure GPIOC */
	/*******************/

	/* Enable GPIOC */

	rcc->AHBENR |= AHBENR_GPIOCEN;	/* Enable GPIOC clock */

	/* Take over PC0 from LCD */

	temp=gpioc->MODER;
	temp&=~0x00000003;		/* clear values for pin 6 */
	temp|= (MODER_INPUT<<(0*2));	/* Set output type to ANALOG */
	gpioc->MODER = temp;

	/* Clock Speed */
	temp=gpioc->OSPEEDR&~0x00000003;
	temp|= 0x00000003;		/* 40MHz clock speed */
	gpioc->OSPEEDR=temp;

	/* Pull-Up/Pull-Down */
	gpioc->PUPDR &=~0x00000003;	/* No pull-up pull-down */

	/* Set output type */
	gpioc->OTYPER &=~0x1;		/* push-pull */


	/************************/
	/* Enable A/D Converter */
	/************************/

	/* Enable Clock */
	rcc->APB2ENR |= RCC_APB2ENR_ADC1EN;

	/* Turn off ADC conversion */
	/* Note possibly we have to wait for adc->SR ADONS==1 before doing */
	adc->CR2 &=~ ADC_CR2_ADON;

	/* Select only one conversion */
	adc->SQR1 &=~ (0xf<<20);	/* set to 0, only 1 conversion */

	/* Set channel 10 as 1st conversion in sequence */
	adc->SQR5 &=~ (0x1f);
	adc->SQR5 |= 0xa;

	/* Configure sample time register for channel 10 */
	adc->SMPR2 |= ADC_SMPR2_SMP10;

	/* resolution to 12bit */
	adc->CR1 &= ~(ADC_CR1_RES_MASK);

	/* Enable end-of-conversion interrupt */
	adc->CR1 |= ADC_CR1_ECOCIE;

	/* Enable continuous conversion mode */
	adc->CR2 |= ADC_CR2_CONT;

	/* Enable ADC interrupt in NVIC */
	nvic->ISER[ADC1_IRQn >> 0x05] =
		(uint32_t)0x01 << (ADC1_IRQn & (uint8_t)0x1F);

	/* Configure delay selection as delayed until read */
	adc->CR2 &=~ADC_CR2_DELS_MASK;
	adc->CR2 |=ADC_CR2_DELS_READ;

	/* Configure interrupt priority? */

	/* Turn on ADC conversion */
	adc->CR2 |= ADC_CR2_ADON;

	/* start the conversion of the regular channels by software */
        adc->CR2   |= ADC_CR2_SWSTART;

	/* Loop forever */

	result=9999;
	string[0]=0;
	string[1]=0;
	string[2]=0;
	string[3]=0;
	string[4]=0;
	string[5]=0;

	int i=0;
	for(;;) {

		int deg_c,deg_f,voltage;

		voltage=(result*3300)/4095;
		deg_c=(voltage*100)-50000;
		deg_f=((deg_c*9)/5)+32000;

		if (i==0) {
			convert_to_temp(voltage,0);
		}
		if (i==1) {
			convert_to_temp(deg_c,1);
		}
		if (i==2) {
			convert_to_temp(deg_f,2);
		}
		if (i==3) {
			convert_result(result);
		}

		i++;
		if (i==4) i=0;

		lcd_convert(string,lcd_buffer);
		lcd_display(lcd_buffer);

		busy_delay(250000);



	}

	return 0;
}



static void adc1_irq_handler(void) {

	result=1234;

	if (adc->SR & ADC_SR_EOC) {

		result=adc->DR;

	}
}


static void nmi_handler(void) {
	for(;;);
}

static void hardfault_handler(void) {
	for(;;);
}

static void do_nothing(void) {
	for(;;);
}


/* Vector Table */

#define STACK_TOP 0x20000800

/* CD00240193.pdf p 190 */

unsigned int *myvectors[]
__attribute__ ((section("vectors"))) = {
	(uint32_t *) STACK_TOP,		/* 0x00 stack pointer      */
	(uint32_t *) main,		/* 0x04 code entry point   */
	(uint32_t *) nmi_handler,	/* 0x08 NMI handler        */
	(uint32_t *) hardfault_handler,	/* 0x0c hard fault handler */
	(uint32_t *) do_nothing,	/* 0x10 mem_manage */
	(uint32_t *) do_nothing,	/* 0x14 bus_fault */
	(uint32_t *) do_nothing,	/* 0x18 usage_fault */
	(uint32_t *) 0,			/* 0x1c reserved */
	(uint32_t *) 0,			/* 0x20 reserved */
	(uint32_t *) 0,			/* 0x24 reserved */
	(uint32_t *) 0,			/* 0x28 reserved */
	(uint32_t *) do_nothing,	/* 0x2c SVC */
	(uint32_t *) do_nothing,	/* 0x30 debug */
	(uint32_t *) 0,			/* 0x34 reserved */
	(uint32_t *) do_nothing,	/* 0x38 pend */
	(uint32_t *) do_nothing,	/* 0x3c timer tick */
	/* External Interrupts */
	(uint32_t *) 0,			/* 0x40  Window Watchdog */
	(uint32_t *) 0,			/* 0x44 PVD through EXTI Line detect */
	(uint32_t *) 0,			/* 0x48 Tamper and Time Stamp */
	(uint32_t *) 0,			/* 0x4c RTC Wakeup */
	(uint32_t *) 0,			/* 0x50 FLASH */
	(uint32_t *) 0,			/* 0x54  RCC */
	(uint32_t *) 0,			/* 0x58 EXTI Line 0 */
	(uint32_t *) 0,			/* 0x5c EXTI Line 1 */
	(uint32_t *) 0,			/* 0x60 EXTI Line 2 */
	(uint32_t *) 0,			/* 0x64 EXTI Line 3 */
	(uint32_t *) 0,			/* 0x68 EXTI Line 4 */
	(uint32_t *) 0,			/* 0x6c DMA1 Channel */
	(uint32_t *) 0,			/* 0x70 DMA1 Channel 2 */
	(uint32_t *) 0,			/* 0x74 DMA1 Channel 3 */
	(uint32_t *) 0,			/* 0x78 DMA1 Channel 4 */
	(uint32_t *) 0,			/* 0x7c DMA1 Channel 5 */
	(uint32_t *) 0,			/* 0x80 DMA1 Channel 6 */
	(uint32_t *) 0,			/* 0x84 DMA1 Channel 7 */
	(uint32_t *) adc1_irq_handler,	/* 0x88 ADC1 */
	(uint32_t *) 0,			/* 0x8c USB High Priority */
	(uint32_t *) 0,			/* 0x90 USB Low  Priority */
	(uint32_t *) 0,			/* 0x94 DAC */
	(uint32_t *) 0,			/* 0x98 COMP through EXTI Line */
	(uint32_t *) 0,			/* 0x9c EXTI Line 9..5 */
	(uint32_t *) 0,			/* 0xa0 LCD */
	(uint32_t *) 0,			/* 0xa4 TIM9 */
	(uint32_t *) 0,			/* 0xa8 TIM10 */
	(uint32_t *) 0,			/* 0xac TIM11 */
	(uint32_t *) 0,			/* 0xb0 TIM2 */
	(uint32_t *) 0,			/* 0xb4 TIM3 */
	(uint32_t *) 0,			/* 0xb8 TIM4 */
};
