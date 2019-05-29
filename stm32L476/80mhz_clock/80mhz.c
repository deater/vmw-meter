/* Test changing clock to 80MHz */

#include <stdint.h>
#include "stm32l476xx.h"

#include "lcd.h"

/* global variables */
volatile uint32_t TimeDelay;


void NVIC_SetPriority(int irq, int priority);
void NVIC_EnableIRQ(int irq);
void System_Clock_Init(void);

/* Interrupt Handlers */
static void TIM4_IRQHandler(void) {


	/* Check if update interrupt happened */
	if ((TIM4->SR & TIM_SR_UIF)!=0) {
#if 0
		led_count++;
		if (led_count==FREQ) {
			if (led_on) {
				led_on=0;
				GPIOB->ODR &= ~(1<<2);
			}
			else {
				led_on=1;
				GPIOB->ODR |= (1<<2);
			}
			led_count=0;
		}
#endif
		/* ACK interrupt */
		TIM4->SR &= ~TIM_SR_UIF;
	}

}

void SysTick_Handler(void) {

	if (TimeDelay > 0) TimeDelay--;
}



static void TIM4_Init(void) {

	/* enable Timer 4 clock */
	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM4EN;

	/* Edge aligned mode */
	TIM4->CR1 &= ~TIM_CR1_CMS;

	/* Counting direction: 0=up, 1=down */
	TIM4->CR1 &= ~TIM_CR1_DIR; // up-counting

	TIM4->CR1 &= ~TIM_CR1_UDIS; // enable update

	/* Master mode selection */
	/* 100 = OC1REF as TRGO */
//	TIM4->CR2 &= ~TIM_CR2_MMS;
//	TIM4->CR2 |= TIM_CR2_MMS_2;

	/* Trigger interrupt enable */
//	TIM4->DIER |= TIM_DIER_TIE;

	/* Update interrupt enable */
	TIM4->DIER |= TIM_DIER_UIE;

	/* OC1M: Output Compare 1 mode */
	/* 0110 = PWM mode 1 */
//	TIM4->CCMR1 &= ~TIM_CCMR1_OC1M;
//	TIM4->CCMR1 |= TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2;

	/* FIXME */
	/* Prescaler. slow down the input clock by a factor of (1+prescaler) */
	TIM4->PSC=54;		// 16MHz / (1+10) = 1.454MHz

	/* Auto-reload, max */
	TIM4->ARR=32;		// 1.454MHz /(33) = roughly 44kHz, 44077

	/* Duty Ratio */
//	TIM4->CCR1 = 50;	// 50%

	/* OC1 signal output on the corresponding output pin*/
//	TIM4->CCER |= TIM_CCER_CC1E;

	/* Enable Timer */
	TIM4->CR1 |= TIM_CR1_CEN;

	/* Set highest priority intterupt */
	NVIC_SetPriority(TIM4_IRQn, 0);

	/* Set highest priority intterupt */
	NVIC_EnableIRQ(TIM4_IRQn);

}

static void GPIOB_Clock_Enable(void) {
        RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
}

static void GPIOB_Pin_Output_Init(int pin) {

        /* Set mode of pin 2 as output */
        /* 00 = input
           01 = output
           10 = alternate
           11 = analog (default) */

        GPIOB->MODER &= ~(3UL<<(pin*2));
        GPIOB->MODER |= 1UL<<(pin*2);

        /* set output type of pin2 as push-pull */
        /* 0 = push-pull (default)
           1 = open-drain */
        GPIOB->OTYPER &= ~(1<<pin);
        /* Set output speed of pin2 as low */
        /* 00 = Low speed
           01 = Medium speed
           10 = Fast speed
           11 = High speed */
        GPIOB->OSPEEDR &= ~(3UL<<(pin*2));

        /* Set pin 2 as no pull-up, no pull-down */
        /* 00 = no pull-up, no pull-down
           01 = pull-up
           10 = pull-down
           11 = reserved */
        GPIOB->PUPDR &=~(3UL<<(pin*2));
}


static void GPIOE_Clock_Enable(void) {
        RCC->AHB2ENR |= RCC_AHB2ENR_GPIOEEN;
}

static void GPIOE_Pin_Output_Init(int pin) {

        /* Set mode of pin 2 as output */
        /* 00 = input
           01 = output
           10 = alternate
           11 = analog (default) */

        GPIOE->MODER &= ~(3UL<<(pin*2));
        GPIOE->MODER |= 1UL<<(pin*2);

        /* set output type of pin2 as push-pull */
        /* 0 = push-pull (default)
           1 = open-drain */
        GPIOE->OTYPER &= ~(1<<pin);
        /* Set output speed of pin2 as low */
        /* 00 = Low speed
           01 = Medium speed
           10 = Fast speed
           11 = High speed */
        GPIOE->OSPEEDR &= ~(3UL<<(pin*2));
       /* Set pin 2 as no pull-up, no pull-down */
        /* 00 = no pull-up, no pull-down
           01 = pull-up
           10 = pull-down
           11 = reserved */
        GPIOE->PUPDR &=~(3UL<<(pin*2));
}



static void GPIOA_Clock_Enable(void) {
        RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
}

static void GPIOA_Pin_Input_Init(int pin) {

        /* Set mode to input 00 */
        /* 00 = input
           01 = output
           10 = alternate
           11 = analog (default) */

        GPIOA->MODER &= ~(3UL<<(pin*2));

        /* Set output speed of pin2 as low */
        /* 00 = Low speed
           01 = Medium speed
           10 = Fast speed
           11 = High speed */
        //GPIOA->OSPEEDR &= ~(3UL<<(pin*2));

        /* Set pin 2 as pull-down */
	/* 00 = no pull-up, no pull-down
           01 = pull-up
           10 = pull-down
           11 = reserved */
        GPIOA->PUPDR &=~(3UL<<(pin*2));
        GPIOA->PUPDR |= (2UL<<(pin*2));
}

int main(void) {


	char buffer[7];
//	volatile int d;
	int led_on=0;

	System_Clock_Init();

	/* Enable GPIO-PA3 -- up on joystick */
	GPIOA_Clock_Enable();
	GPIOA_Pin_Input_Init(3);

	/* RED LED is GPIO-PB2 */
        GPIOB_Clock_Enable();
        GPIOB_Pin_Output_Init(2);

        /* GREEN LED is GPIO-PE8 */
        GPIOE_Clock_Enable();
        GPIOE_Pin_Output_Init(8);

	/* Set up LCD */
	LCD_Clock_Init();
	LCD_Pin_Init();
	LCD_Configure();

//	TIM4_Init();
	asm volatile ( "cpsie i" );

	LCD_Display_String("AWESOME");

	while(1) {

		/* Blink GREEN LED (GPIOE8) based on note B */
		if (led_on) {
			GPIOE->ODR &= ~(1<<8);
			led_on=0;
		}
		else {
			GPIOE->ODR |= (1<<8);
			led_on=1;
		}

	}


}

/* Note: no need to touch the code beyond this point */

unsigned int krg=0;

/* Set 80MHz PLL clock */
void System_Clock_Init(void) {

	/* Set the FLASH latency to 4 (???) */
	FLASH->ACR|=FLASH_ACR_LATENCY_4;

	/* Note, this code initializes the HSI 16MHz clock */
	/* Also multplies it by ??? for the system clock */

        /* Enable the HSI clock */
        RCC->CR |= RCC_CR_HSION;
	/* Wait until HSI is ready */
	while ( (RCC->CR & RCC_CR_HSIRDY) == 0 );

        /* Enable the HSI clock */
//        RCC->CR |= RCC_CR_MSION;
	/* Wait until HSI is ready */
//	while ( (RCC->CR & RCC_CR_MSIRDY) == 0 );


	RCC->CR &= ~RCC_CR_PLLON;
	while((RCC->CR & RCC_CR_PLLRDY) == RCC_CR_PLLRDY);

	RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLSRC;
	RCC->PLLCFGR |= RCC_PLLCFGR_PLLSRC_MSI;

#if 1
	/* set up the PLL */
/*
	The system Clock is configured as follows :
            System Clock source            = PLL (MSI)
            AHB Prescaler                  = 1
            APB1 Prescaler                 = 1
            APB2 Prescaler                 = 1
            PLL_M                          = 1
            PLL_N                          = 40
            PLL_R                          = 2
	( SYSCLK = PLLCLK = VCO=(4MHz)*N = 160 /M/R = 80 MHz )
            PLL_P                          = 7 (No reason for this...)
            PLL_Q                          = 4
		 ( PLL48M1CLK = VCO /M/Q) = 40 MHz ?! )
            Flash Latency(WS)              = 4
*/

	// SYSCLK = (IN*N)/M

	/* PLL_N=40 --- VCO=IN*PLL_N */
	RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLN;		// set to HSI*20=320MHz
	RCC->PLLCFGR |= 40<<8;

	/* PLL_M=1 (00:M=1 01:M=2, 10:M=3, etc) */
	RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLM;		// 320MHz/2 = 160MHz
//	RCC->PLLCFGR |= 1<<4;

	/* PLL_R =2 (00=2, 01=4 10=6 11=8) */
	RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLR;		// 160MHz/2 = 80MHz
//	RCC->PLLCFGR |= 1<<25;

	/* PLL_P=7 (register=0 means 7, 1=17) for SAI1/SAI2 clock */
//	RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLP;

	/* PLL_Q=8 (11) sets 48MHz clock to 320/8=40? */
//	RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLQ;
//	RCC->PLLCFGR |= 3<<21;

	RCC->CR |= RCC_CR_PLLON;
	while (!(RCC->CR & RCC_CR_PLLRDY)) ;

	RCC->PLLCFGR |= RCC_PLLCFGR_PLLREN;

	/* Select PLL as system clock source  */
	RCC->CFGR &= ~RCC_CFGR_SW;
	RCC->CFGR |= RCC_CFGR_SW_PLL;  /* 11: PLL used as sys clock */
	while ((RCC->CFGR & RCC_CFGR_SWS)!=RCC_CFGR_SWS_PLL);

#else

	/* Select HSI as system clock source  */
	RCC->CFGR &= ~RCC_CFGR_SW;
	RCC->CFGR |= RCC_CFGR_SW_HSI;  /* 01: HSI16 oscillator used as system clock */
#endif
	/* Wait till HSI is used as system clock source */
//	while ((RCC->CFGR & RCC_CFGR_SWS) == 0 );

	krg=RCC->PLLCFGR;

}


/* Set 80MHz PLL clock */
void System_Clock_Init_Bad(void) {

	/* Note, this code initializes the HSI 16MHz clock */
	/* Also multplies it by ??? for the system clock */

        /* Enable the HSI clock */
        RCC->CR |= RCC_CR_HSION;

	/* Wait until HSI is ready */
	while ( (RCC->CR & RCC_CR_HSIRDY) == 0 );

	RCC->CR &= ~RCC_CR_PLLON;
	while((RCC->CR & RCC_CR_PLLRDY) == RCC_CR_PLLRDY);

	RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLSRC;
	RCC->PLLCFGR |= RCC_PLLCFGR_PLLSRC_HSI;

#if 1
	/* set up the PLL */
/*
	The system Clock is configured as follows :
            System Clock source            = PLL (HSI)
            AHB Prescaler                  = 1
            APB1 Prescaler                 = 1
            APB2 Prescaler                 = 1
            PLL_M                          = 2
            PLL_N                          = 20
            PLL_R                          = 2
	( SYSCLK = PLLCLK = VCO=(16MHz)*N = 320 /M/R = 80 MHz )
            PLL_P                          = 7 (No reason for this...)
            PLL_Q                          = 8
		 ( PLL48M1CLK = VCO /M/Q) = 40 MHz ?! )
            Flash Latency(WS)              = 4
*/

	// SYSCLK = (IN*N)/M

	/* PLL_N=20 --- VCO=IN*PLL_N */
	RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLN;		// set to HSI*20=320MHz
	RCC->PLLCFGR |= 20<<8;

	/* PLL_M=2 (00:M=1 01:M=2, 10:M=3, etc) */
	RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLM;		// 320MHz/2 = 160MHz
	RCC->PLLCFGR |= 1<<4;

	/* PLL_R =2 (00=2, 01=4 10=6 11=8) */
	RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLR;		// 160MHz/2 = 80MHz
	RCC->PLLCFGR |= 1<<25;

	/* PLL_P=7 (register=0 means 7, 1=17) for SAI1/SAI2 clock */
//	RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLP;

	/* PLL_Q=8 (11) sets 48MHz clock to 320/8=40? */
//	RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLQ;
//	RCC->PLLCFGR |= 3<<21;

	RCC->CR |= RCC_CR_PLLON;
	while (!(RCC->CR & RCC_CR_PLLRDY)) ;

	RCC->PLLCFGR |= RCC_PLLCFGR_PLLREN;

	/* Select PLL as system clock source  */
	RCC->CFGR &= ~RCC_CFGR_SW;
	RCC->CFGR |= RCC_CFGR_SW_PLL;  /* 11: PLL used as sys clock */
	while ((RCC->CFGR & RCC_CFGR_SWS)!=RCC_CFGR_SWS_PLL);

#else

	/* Select HSI as system clock source  */
	RCC->CFGR &= ~RCC_CFGR_SW;
	RCC->CFGR |= RCC_CFGR_SW_HSI;  /* 01: HSI16 oscillator used as system clock */
#endif
	/* Wait till HSI is used as system clock source */
//	while ((RCC->CFGR & RCC_CFGR_SWS) == 0 );

	krg=RCC->PLLCFGR;

}

void NVIC_SetPriority(int irq, int priority) {

	if (irq<0) {
		// for -1, 0xff -> f -> -4 = b (11)
		SCB->SHP[(((uint8_t)irq)&0xf)-4]=(priority<<4)&0xff;
	}
	else {
		NVIC->IP[irq]=(priority<<4)&0xff;
	}

	return;
}

void NVIC_EnableIRQ(int irq) {

	NVIC->ISER[irq >> 5] = (1UL << (irq & 0x1F));

	return;
}



static void nmi_handler(void) {
	for(;;);
}

static void hardfault_handler(void) {
	for(;;);
}

extern unsigned long _etext,_data,_edata,_bss_start,_bss_end;

	/* Copy DATA and BSS segments into RAM */
void Reset_Handler(void)	{

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

#define STACK_LOCATION (0x20000000+(96*1024))

/* Vector Table */
uint32_t *myvectors[256]
__attribute__ ((section(".isr_vector"))) = {
	(uint32_t *) STACK_LOCATION,	/*   0:00  = stack pointer	*/
	(uint32_t *) Reset_Handler,	/* -15:04  = code entry point	*/
	(uint32_t *) nmi_handler,	/* -14:08  = NMI handler	*/
	(uint32_t *) hardfault_handler,	/* -13:0c = hard fault handler	*/
	(uint32_t *) nmi_handler,	/* -12:10 = MemManage		*/
	(uint32_t *) nmi_handler,	/* -11:14 = BusFault		*/
	(uint32_t *) nmi_handler,	/* -10:18 = UsageFault		*/
	(uint32_t *) nmi_handler,	/*  -9:1c = Reserved		*/
	(uint32_t *) nmi_handler,	/*  -8:20 = Reserved		*/
	(uint32_t *) nmi_handler,	/*  -7:24 = Reserved		*/
	(uint32_t *) nmi_handler,	/*  -6:28 = Reserved		*/
	(uint32_t *) nmi_handler,	/*  -5:2c = SVC Handler		*/
	(uint32_t *) nmi_handler,	/*  -4:30 = Debugmon		*/
	(uint32_t *) nmi_handler,	/*  -3:34 = Reserved		*/
	(uint32_t *) nmi_handler,	/*  -2:38 = PendSV		*/
	(uint32_t *) SysTick_Handler,	/*  -1:3c = SysTick		*/
	(uint32_t *) nmi_handler,	/*   0:40 = WWDG		*/
	(uint32_t *) nmi_handler,	/*   1:44 = PVD_PVM		*/
	(uint32_t *) nmi_handler,	/*   2:48 = RTC_TAMP_STAMP	*/
	(uint32_t *) nmi_handler,	/*   3:4C = RTC_WKUP		*/
	(uint32_t *) nmi_handler,	/*   4:50 = FLASH		*/
	(uint32_t *) nmi_handler,	/*   5:54 = RCC			*/
	(uint32_t *) nmi_handler,	/*   6:58 = EXTI0		*/
	(uint32_t *) nmi_handler,	/*   7:5c = EXTI1		*/
	(uint32_t *) nmi_handler,	/*   8:60 = EXTI2		*/
	(uint32_t *) nmi_handler,	/*   9:64 = EXTI3		*/
	(uint32_t *) nmi_handler,	/*  10:68 = EXTI4		*/
	(uint32_t *) nmi_handler,	/*  11:6C = DMA1_CH1		*/
	(uint32_t *) nmi_handler,	/*  12:70 = DMA1_CH2		*/
	(uint32_t *) nmi_handler,	/*  13:74 = DMA1_CH3		*/
	(uint32_t *) nmi_handler,	/*  14:78 = DMA1_CH4		*/
	(uint32_t *) nmi_handler,	/*  15:7c = DMA1_CH5		*/
	(uint32_t *) nmi_handler,	/*  16:80 = DMA1_CH6		*/
	(uint32_t *) nmi_handler,	/*  17:84 = DMA1_CH7		*/
	(uint32_t *) nmi_handler,	/*  18:84 = ADC1_2		*/
	(uint32_t *) nmi_handler,	/*  19:88 = CAN1_TX		*/
	(uint32_t *) nmi_handler,	/*  20:90 = CAN1_RX0		*/
	(uint32_t *) nmi_handler,	/*  21:94 = CAN1_RX1		*/
	(uint32_t *) nmi_handler,	/*  22:98 = CAN1_SCE		*/
	(uint32_t *) nmi_handler,	/*  23:9C = EXTI9_5		*/
	(uint32_t *) nmi_handler,	/*  24:A0 = TIM1_BRK/TIM15	*/
	(uint32_t *) nmi_handler,	/*  25:A4 = TIM1_UP/TIM16	*/
	(uint32_t *) nmi_handler,	/*  26:A8 = TIM1_TRG_COM/TIM17	*/
	(uint32_t *) nmi_handler,	/*  27:AC = TIM1_CC		*/
	(uint32_t *) nmi_handler,	/*  28:B0 = TIM2		*/
	(uint32_t *) nmi_handler,	/*  29:B4 = TIM3		*/
	(uint32_t *) TIM4_IRQHandler,	/*  30:B8 = TIM4		*/
	(uint32_t *) nmi_handler,	/*  31:BC = I2C1_EV		*/
	(uint32_t *) nmi_handler,	/*  32:C0 = I2C1_ER		*/
	(uint32_t *) nmi_handler,	/*  33:C4 = I2C2_EV		*/
	(uint32_t *) nmi_handler,	/*  34:C8 = I2C2_ER		*/
	(uint32_t *) nmi_handler,	/*  35:CC = SPI1		*/
	(uint32_t *) nmi_handler,	/*  36:D0 = SPI2		*/
	(uint32_t *) nmi_handler,	/*  37:D4 = USART1		*/
	(uint32_t *) nmi_handler,	/*  38:D8 = USART2		*/
	(uint32_t *) nmi_handler,	/*  39:DC = USART3		*/
	(uint32_t *) nmi_handler,	/*  40:E0 = EXTI5_10		*/
	(uint32_t *) nmi_handler,	/*  41:E4 = RTC_ALART		*/
	(uint32_t *) nmi_handler,	/*  42:E8 = DFSDM1_FLT3		*/
	(uint32_t *) nmi_handler,	/*  43:EC = TIM8_BRK		*/
	(uint32_t *) nmi_handler,	/*  44:F0 = TIM8_UP		*/
	(uint32_t *) nmi_handler,	/*  45:F4 = TIM8_TRG_COM	*/
	(uint32_t *) nmi_handler,	/*  46:F8 = TIM8_CC		*/
	(uint32_t *) nmi_handler,	/*  47:FC = ADC3		*/
	(uint32_t *) nmi_handler,	/*  48:100 = FMC		*/
	(uint32_t *) nmi_handler,	/*  49:104 = SDMMC1		*/
	(uint32_t *) nmi_handler,	/*  50:108 = TIM5		*/
	(uint32_t *) nmi_handler,	/*  51:10C = SPI3		*/
	(uint32_t *) nmi_handler,	/*  52:110 = UART4		*/
	(uint32_t *) nmi_handler,	/*  53:114 = UART5		*/
	(uint32_t *) nmi_handler,	/*  54:118 = TIM6_DACUNDER	*/
	(uint32_t *) nmi_handler,	/*  55:11C = TIM7		*/
	(uint32_t *) nmi_handler,	/*  56:120 = DMA2_CH1		*/
	(uint32_t *) nmi_handler,	/*  57:124 = DMA2_CH2		*/
	(uint32_t *) nmi_handler,	/*  58:128 = DMA2_CH3		*/
	(uint32_t *) nmi_handler,	/*  59:12C = DMA2_CH4		*/
	(uint32_t *) nmi_handler,	/*  60:130 = DMA2_CH5		*/
	(uint32_t *) nmi_handler,	/*  61:134 = DFSDM1_FLT0	*/
	(uint32_t *) nmi_handler,	/*  62:138 = DFSDM1_FLT1	*/
	(uint32_t *) nmi_handler,	/*  63:13C = DFSDM1_FLT2	*/
	(uint32_t *) nmi_handler,	/*  64:140 = COMP		*/
	(uint32_t *) nmi_handler,	/*  65:144 = LPTIM1		*/
	(uint32_t *) nmi_handler,	/*  66:148 = LPTIM2		*/
	(uint32_t *) nmi_handler,	/*  67:14C = OTG_FS		*/
	(uint32_t *) nmi_handler,	/*  68:150 = DMA2_CH6		*/
	(uint32_t *) nmi_handler,	/*  69:154 = DMA2_CH7		*/
	(uint32_t *) nmi_handler,	/*  70:158 = LPUART1		*/
	(uint32_t *) nmi_handler,	/*  71:15C = QUADSPI		*/
	(uint32_t *) nmi_handler,	/*  72:160 = I2C3_EV		*/
	(uint32_t *) nmi_handler,	/*  73:164 = I2C3_ER		*/
	(uint32_t *) nmi_handler,	/*  74:168 = SAI1		*/
	(uint32_t *) nmi_handler,	/*  75:16C = SAI2		*/
	(uint32_t *) nmi_handler,	/*  76:170 = SWPMI1		*/
	(uint32_t *) nmi_handler,	/*  77:174 = TSC		*/
	(uint32_t *) nmi_handler,	/*  78:178 = LCD		*/
	(uint32_t *) nmi_handler,	/*  79:17C = AES		*/
	(uint32_t *) nmi_handler,	/*  80:180 = RNG		*/
	(uint32_t *) nmi_handler,	/*  81:184 = FPU		*/
};
