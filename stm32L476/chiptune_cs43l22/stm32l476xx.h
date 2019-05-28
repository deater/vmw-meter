/* In en.DM00083560.pdf:   */

#define __I volatile const	/* read only */
#define __O volatile		/* write only */
#define __IO volatile		/* read-write */


/* p77 */
#define PERIPH_BASE	((uint32_t)0x40000000)
#define APB1PERIPH_BASE (PERIPH_BASE)
#define APB2PERIPH_BASE (PERIPH_BASE + 0x00010000)
#define AHB1PERIPH_BASE	(PERIPH_BASE + 0x00020000)
#define AHB2PERIPH_BASE	(PERIPH_BASE + 0x08000000)

/* 6.4 p223 */
/* Reset and Clock Control */
typedef struct {
__IO uint32_t CR;		/* 0x00 */
__IO uint32_t ICSCR;		/* 0x04 */
__IO uint32_t CFGR;		/* 0x08 */
__IO uint32_t PLLCFGR;		/* 0x0c */
__IO uint32_t PLLSAI1CFGR;	/* 0x10 */
__IO uint32_t PLLSAI2CFGR;	/* 0x14 */
__IO uint32_t CIER;		/* 0x18 */
__IO uint32_t CIFR;		/* 0x1c */
__IO uint32_t CICR;		/* 0x20 */
__IO uint32_t RESERVED0;	/* 0x24 */
__IO uint32_t AHB1RSTR;		/* 0x28 */
__IO uint32_t AHB2RSTR;		/* 0x2c */
__IO uint32_t AHB3RSTR;		/* 0x30 */
__IO uint32_t RESERVED1;	/* 0x34 */
__IO uint32_t APB1RSTR1;	/* 0x38 */
__IO uint32_t APB1RSTR2;	/* 0x3c */
__IO uint32_t APB2RSTR;		/* 0x40 */
__IO uint32_t RESERVED2;	/* 0x44 */
__IO uint32_t AHB1ENR;		/* 0x48 */
__IO uint32_t AHB2ENR;		/* 0x4c */
__IO uint32_t AHB3ENR;		/* 0x50 */
__IO uint32_t RESERVED3;	/* 0x54 */
__IO uint32_t APB1ENR1;		/* 0x58 */
__IO uint32_t APB1ENR2;		/* 0x5c */
__IO uint32_t APB2ENR;		/* 0x60 */
__IO uint32_t RESERVED4;	/* 0x64 */
__IO uint32_t AHB1SMENR;	/* 0x68 */
__IO uint32_t AHB2SMENR;	/* 0x6C */
__IO uint32_t AHB3SMENR;	/* 0x70 */
__IO uint32_t RESERVED5;	/* 0x74 */
__IO uint32_t APB1SMENR1;	/* 0x78 */
__IO uint32_t APB1SMENR2;	/* 0x7C */
__IO uint32_t APB2SMENR;	/* 0x80 */
__IO uint32_t RESERVED6;	/* 0x84 */
__IO uint32_t CCIPR;		/* 0x88 */
__IO uint32_t RESERVED7;	/* 0x8C */
__IO uint32_t BDCR;		/* 0x90 */
__IO uint32_t CSR;		/* 0x94 */

} RCC_TypeDef;

#define RCC_CR_MSION		(1<<0)
#define RCC_CR_MSIRDY		(1<<1)
#define RCC_CR_MSIPLLEN		(1<<2)
#define RCC_CR_MSIRGSEL		(1<<3)
#define RCC_CR_MSIRANGE		(0xf<<4)
#define RCC_CR_HSION		(1<<8)
#define RCC_CR_HSIKERON		(1<<9)
#define RCC_CR_HSIRDY		(1<<10)
#define RCC_CR_HSIASFS		(1<<11)
#define RCC_CR_HSEON		(1<<16)
#define RCC_CR_HSERDY		(1<<17)
#define RCC_CR_HSEBYP		(1<<18)
#define RCC_CR_PLLON		(1<<24)
#define RCC_CR_PLLRDY		(1<<25)


#define RCC_CFGR_SW		(0x3)
#define RCC_CFGR_SW_PLL		(0x3)
#define RCC_CFGR_SW_HSE		(0x2)
#define RCC_CFGR_SW_HSI		(0x1)
#define RCC_CFGR_SW_MSI		(0x0)
#define RCC_CFGR_HPRE		(0xf<<4)

#define RCC_CFGR_SWS		(0x3<<2)
#define RCC_CFGR_SWS_PLL	(0x3<<2)

#define RCC_PLLCFGR_PLLPDIV	(0x1f<<27)
#define RCC_PLLCFGR_PLLR	(0x3<<25)
#define RCC_PLLCFGR_PLLREN	(0x1<<24)
#define RCC_PLLCFGR_PLLQ	(0x3<<21)
#define RCC_PLLCFGR_PLLQEN	(0x1<<20)
#define RCC_PLLCFGR_PLLP	(0x1<<17)
#define RCC_PLLCFGR_PLLPEN	(0x1<<16)
#define RCC_PLLCFGR_PLLN	(0xff<<8)
#define RCC_PLLCFGR_PLLM	(0x7<<4)

#define RCC_PLLCFGR_PLLSRC	(0x3)
#define RCC_PLLCFGR_PLLSRC_HSI	(0x2)

/* 6.4.13 p246 */
#define RCC_APB1RSTR1_LPTIM1RST	(1<<31)
#define RCC_APB1RSTR1_OPAMPRST	(1<<30)
#define RCC_APB1RSTR1_DAC1RST	(1<<29)
#define RCC_APB1RSTR1_PWRRST	(1<<28)
#define RCC_APB1RSTR1_CAN2RST	(1<<26)
#define RCC_APB1RSTR1_CAN1RST	(1<<25)
#define RCC_APB1RSTR1_CRSRST	(1<<24)
#define RCC_APB1RSTR1_I2C3RST	(1<<23)
#define RCC_APB1RSTR1_I2C2RST	(1<<22)
#define RCC_APB1RSTR1_I2C1RST	(1<<21)
#define RCC_APB1RSTR1_UART5RST	(1<<20)
#define RCC_APB1RSTR1_UART4RST	(1<<19)
#define RCC_APB1RSTR1_UART3RST	(1<<18)
#define RCC_APB1RSTR1_UART2RST	(1<<17)
#define RCC_APB1RSTR1_SPI3RST	(1<<15)
#define RCC_APB1RSTR1_SPI2RST	(1<<14)
#define RCC_APB1RSTR1_LCDRST	(1<<9)
#define RCC_APB1RSTR1_TIM7RST	(1<<5)
#define RCC_APB1RSTR1_TIM6RST	(1<<4)
#define RCC_APB1RSTR1_TIM5RST	(1<<3)
#define RCC_APB1RSTR1_TIM4RST	(1<<2)
#define RCC_APB1RSTR1_TIM3RST	(1<<1)
#define RCC_APB1RSTR1_TIM2RST	(1<<0)

/* p251 */
#define RCC_AHB2ENR_RNGEN	(1<<18)
#define RCC_AHB2ENR_HASHEN	(1<<17)
#define RCC_AHB2ENR_AESEN	(1<<16)
#define RCC_AHB2ENR_DCMIEN	(1<<14)
#define RCC_AHB2ENR_ADCEN	(1<<13)
#define RCC_AHB2ENR_OTGFSEN	(1<<12)
#define RCC_AHB2ENR_GPIOIEN	(1<<8)
#define RCC_AHB2ENR_GPIOHEN	(1<<7)
#define RCC_AHB2ENR_GPIOGEN	(1<<6)
#define RCC_AHB2ENR_GPIOFEN	(1<<5)
#define RCC_AHB2ENR_GPIOEEN	(1<<4)
#define RCC_AHB2ENR_GPIODEN	(1<<3)
#define RCC_AHB2ENR_GPIOCEN	(1<<2)
#define RCC_AHB2ENR_GPIOBEN	(1<<1)
#define RCC_AHB2ENR_GPIOAEN	(1<<0)

/* 6.4.19, p253 */
#define RCC_APB1ENR1_TIM2EN	(1<<0)
#define RCC_APB1ENR1_TIM3EN	(1<<1)
#define RCC_APB1ENR1_TIM4EN	(1<<2)
#define RCC_APB1ENR1_TIM5EN	(1<<3)
#define RCC_APB1ENR1_TIM6EN	(1<<4)
#define RCC_APB1ENR1_TIM7EN	(1<<5)
#define RCC_APB1ENR1_LCDEN	(1<<9)
#define RCC_APB1ENR1_RTCAPBEN	(1<<10)
#define RCC_APB1ENR1_WWDGEN	(1<<11)
#define RCC_APB1ENR1_SPI2EN	(1<<14)
#define RCC_APB1ENR1_SPI3EN	(1<<15)
#define RCC_APB1ENR1_UART2EN	(1<<17)
#define RCC_APB1ENR1_UART3EN	(1<<18)
#define RCC_APB1ENR1_UART4EN	(1<<19)
#define RCC_APB1ENR1_UART5EN	(1<<20)
#define RCC_APB1ENR1_I2C1EN	(1<<21)
#define RCC_APB1ENR1_I2C2EN	(1<<22)
#define RCC_APB1ENR1_I2C3EN	(1<<23)
#define RCC_APB1ENR1_CRSEN	(1<<24)
#define RCC_APB1ENR1_CAN1EN	(1<<25)
#define RCC_APB1ENR1_CAN2EN	(1<<26)
#define RCC_APB1ENR1_PWREN	(1<<28)
#define RCC_APB1ENR1_DAC1EN	(1<<29)
#define RCC_APB1ENR1_OPAMPEN	(1<<30)
#define RCC_APB1ENR1_LPTIM1EN	(1<<31)

/* 6.4.21 p259 */
#define RCC_APB2ENR_SDMMC1EN	(1<<10)
#define RCC_APB2ENR_TIM1EN	(1<<11)
#define RCC_APB2ENR_SPI1EN	(1<<12)
#define RCC_APB2ENR_TIM8EN	(1<<13)
#define RCC_APB2ENR_UART1EN	(1<<14)
#define RCC_APB2ENR_TIM15EN	(1<<16)
#define RCC_APB2ENR_TIM16EN	(1<<17)
#define RCC_APB2ENR_TIM17EN	(1<<18)
#define RCC_APB2ENR_SAI1EN	(1<<21)
#define RCC_APB2ENR_SAI2EN	(1<<22)
#define RCC_APB2ENR_DFSDM1EN	(1<<24)

/* 6.4.28, p269 */
#define RCC_CCIPR_I2C2SEL	(0x3<<14)
#define RCC_CCIPR_I2C1SEL	(0x3<<12)

#define RCC_CCIPR_I2C1SEL_0	(1<<12)
#define RCC_CCIPR_I2C1SEL_1	(1<<13)
#define RCC_CCIPR_I2C2SEL_0	(1<<14)
#define RCC_CCIPR_I2C2SEL_1	(1<<15)


/* 6.4.29, p272 */
#define RCC_BDCR_LSEON		(0x1<<0)	/* LSE Oscillator Enable */
#define RCC_BDCR_LSERDY		(0x1<<1)	/* LSE Oscillator Ready */
#define RCC_BDCR_LSEBYP		(0x1<<2)	/* LSE Oscillator Bypass */
#define RCC_BDCR_RTCSEL		(0x3<<8)
#define RCC_BDCR_RTCSEL_LSE	(0x1<<8)
#define RCC_BDCR_RTCSEL_LSI	(0x2<<8)
#define RCC_BDCR_RTCSEL_HSE	(0x3<<8)
#define RCC_BDCR_RTCEN		(0x1<<15)
#define RCC_BDCR_BDRST		(0x1<<16)	/* Backup domain sw reset */

#define	RCC_CSR_MSISRANGE	(0xf<<8)
#define RCC_CSR_LSION		(1<<0)


#define RCC_BASE	(AHB1PERIPH_BASE + 0x1000)
#define RCC		((RCC_TypeDef *)(RCC_BASE))

/* General Purpose IO */
typedef struct {
__IO uint32_t MODER;	/* 0x00 = Mode Register */
__IO uint32_t OTYPER;   /* 0x04 = Output Type Register */
__IO uint32_t OSPEEDR;	/* 0x08 = Output Speed Register */
__IO uint32_t PUPDR;	/* 0x0C = Pull Up/Pull Down Register */
__IO uint32_t IDR;	/* 0x10 = Input Data Register */
__IO uint32_t ODR;	/* 0x14 = Output Data Register */
__IO uint32_t BSRR;	/* 0x18 = Bit Set/Reset Register (LOW) */
__IO uint32_t LCKR;	/* 0x1C = Configuration Lock Register */
__IO uint32_t AFR[2];	/* 0x20 = Alternate Function (Low/High) Registers */
__IO uint32_t BRR;
__IO uint32_t ASCR;
} GPIO_TypeDef;

#define GPIOA_BASE	(AHB2PERIPH_BASE + 0x0000)
#define GPIOA ((GPIO_TypeDef *)GPIOA_BASE)

#define GPIOB_BASE	(AHB2PERIPH_BASE + 0x0400)
#define GPIOB ((GPIO_TypeDef *)GPIOB_BASE)

#define GPIOC_BASE	(AHB2PERIPH_BASE + 0x0800)
#define GPIOC ((GPIO_TypeDef *)GPIOC_BASE)

#define GPIOD_BASE	(AHB2PERIPH_BASE + 0x0C00)
#define GPIOD ((GPIO_TypeDef *)GPIOD_BASE)

#define GPIOE_BASE	(AHB2PERIPH_BASE + 0x1000)
#define GPIOE ((GPIO_TypeDef *)GPIOE_BASE)

#define MODER_INPUT	0x00
#define MODER_OUTPUT	0x01
#define MODER_ALT	0x02
#define MODER_ANALOG	0x03

#define MODER_SET(_s,_v)	(((_v&0x3) << (_s*2))

#define FLASH_BASE	((uint32_t)0x08000000U)

#define RESET	0

/* SYSCFG -- Chapter 9 starting p313 */
/* Register definitions start at */

typedef struct {
__IO uint32_t MEMRMP;	/* 0x00 = memory remap */
__IO uint32_t CFGR1;	/* 0x04 = config register 1 */
__IO uint32_t EXTICR1;	/* 0x08 = external irq config register 1 */
__IO uint32_t EXTICR2;	/* 0x0c = external irq config register 2 */
__IO uint32_t EXTICR3;	/* 0x10 = external irq config register 3 */
__IO uint32_t EXTICR4;	/* 0x14 = external irq config register 4 */
__IO uint32_t SCSR;	/* 0x18 = SRAM2 control register */
__IO uint32_t CFGR2;	/* 0x1c = config register 2 */
__IO uint32_t SWPR;	/* 0x20 = SRAM2 write protect register */
__IO uint32_t SKR;	/* 0x24 = SRAM2 key register */
__IO uint32_t SWPR2;	/* 0x28 = SRAM2 write protect register 2 */
} SYSCFG_TypeDef;

/* p314 */
#define SYSCFG_CFGR1_FPU_IE		(0x3f<<26)	// FPU interrupt enable
#define SYSCFG_CFGR1_I2C4_FMP		(1<<23)		// i2c fast mode
#define SYSCFG_CFGR1_I2C3_FMP		(1<<22)		// i2c fast mode
#define SYSCFG_CFGR1_I2C2_FMP		(1<<21)		// i2c fast mode
#define SYSCFG_CFGR1_I2C1_FMP		(1<<20)		// i2c fast mode
#define SYSCFG_CFGR1_I2C_PB9_FMP	(1<<19)		// i2c fast mode
#define SYSCFG_CFGR1_I2C_PB8_FMP	(1<<18)		// i2c fast mode
#define SYSCFG_CFGR1_I2C_PB7_FMP	(1<<17)		// i2c fast mode
#define SYSCFG_CFGR1_I2C_PB6_FMP	(1<<16)		// i2c fast mode
#define SYSCFG_CFGR1_BOOSTEN		(1<<8)		// I/O switch voltage boost
#define SYSCFG_CFGR1_FWDIS		(1<<0)		// Firewall disable

/* p84 */
#define SYSCFG_BASE	(APB2PERIPH_BASE + 0x0)
#define SYSCFG		((SYSCFG_TypeDef *)(SYSCFG_BASE))

/* ADC -- Chapter 18 starting p504 */
/*	register definitions start in 18.6 on p585 */
typedef struct {
__IO uint32_t ISR;	/* 0x00 = Interrupt and Status Register */
__IO uint32_t IER;	/* 0x04 = Interrupt Enable Register */
__IO uint32_t CR;	/* 0x08 = Control Register */
__IO uint32_t CFGR;	/* 0x0C = Config Register */
__IO uint32_t CFGR2;	/* 0x10 = Config Register2 */
__IO uint32_t SMPR1;	/* 0x14 = Sample Time Register1 */
__IO uint32_t SMPR2;	/* 0x18 = Sample Time Register2 */
__IO uint32_t RESERVED1;/* 0x1C */
__IO uint32_t TR1;	/* 0x20 = Threshold Register1 */
__IO uint32_t TR2;	/* 0x24 = Threshold Register2 */
__IO uint32_t TR3;	/* 0x28 = Threshold Register3 */
__IO uint32_t RESERVED2;/* 0x1C */
__IO uint32_t SQR1;	/* 0x30 = Regular Sequence Register1 */
__IO uint32_t SQR2;	/* 0x34 = Regular Sequence Register2 */
__IO uint32_t SQR3;	/* 0x38 = Regular Sequence Register3 */
__IO uint32_t SQR4;	/* 0x3C = Regular Sequence Register4 */
__IO uint32_t DR;	/* 0x40 = Regular Data Register */
__IO uint32_t RESERVED3;/* 0x44 */
__IO uint32_t RESERVED4;/* 0x48 */
__IO uint32_t JSQR;	/* 0x4C = Injected Sequence Register */
__IO uint32_t RESERVED5[4]; /* 50-5c */
__IO uint32_t OFR1;	/* 0x60 = Offset register */
__IO uint32_t OFR2;	/* 0x64 = Offset register */
__IO uint32_t OFR3;	/* 0x68 = Offset register */
__IO uint32_t OFR4;	/* 0x6C = Offset register */
__IO uint32_t RESERVED6[4]; /* 70-7c */
__IO uint32_t JDR1;	/* 0x80 = Injected Channel 1 register */
__IO uint32_t JDR2;	/* 0x84 = Injected Channel 2 register */
__IO uint32_t JDR3;	/* 0x88 = Injected Channel 3 register */
__IO uint32_t JDR4;	/* 0x8C = Injected Channel 4 register */
__IO uint32_t RESERVED7[4]; /* 90-9c */
__IO uint32_t AWD2CR;	/* 0xA0 = Analog Watchdog 2 */
__IO uint32_t AWD3CR;	/* 0xA4 = Analog Watchdog 3 */
__IO uint32_t RESERVED8;/* 0xA8 */
__IO uint32_t RESERVED9;/* 0x4C */
__IO uint32_t DIFSEL;	/* 0xB0 = Differential mode select register */
__IO uint32_t CALFACT;	/* 0xB4 = Calibration Factors */

} ADC_TypeDef;

#define ADC1_BASE		(AHB2PERIPH_BASE + 0x8040000)
#define ADC2_BASE		(AHB2PERIPH_BASE + 0x8040100)
#define ADC3_BASE		(AHB2PERIPH_BASE + 0x8040200)
#define ADC1 ((ADC_TypeDef *)ADC1_BASE)
#define ADC2 ((ADC_TypeDef *)ADC2_BASE)
#define ADC3 ((ADC_TypeDef *)ADC3_BASE)

/* p 585 */
#define ADC_ISR_JQOVF	(1<<10)
#define ADC_ISR_AWD3	(1<<9)
#define ADC_ISR_AWD2	(1<<8)
#define ADC_ISR_AWD1	(1<<7)
#define ADC_ISR_JEOS	(1<<6)
#define ADC_ISR_JEOC	(1<<5)
#define ADC_ISR_OVR	(1<<4)
#define ADC_ISR_EOS	(1<<3)
#define ADC_ISR_EOC	(1<<2)
#define ADC_ISR_EOSMP	(1<<1)
#define ADC_ISR_ADRDY	(1<<0)


/* p 589 */
#define ADC_CR_ADCAL	(1<<31)		// ADC calibration
#define ADC_ADCALDIF	(1<<30)		// ADC differential calibration
#define ADC_CR_DEEPPWD	(1<<29)		// ADC Deep power down enable
#define ADC_CR_ADVREGEN	(1<<28)		// ADC voltage regulator enable
#define ADC_CR_JADSTP	(1<<5)		// ADC stop injected conversion
#define ADC_CR_ADSTP	(1<<4)		// ADC stop regular conversion
#define ADC_CR_JADSTART	(1<<3)		// ADC start injected conversion
#define ADC_CR_ADSTART	(1<<2)		// ADC start regular conversion
#define ADC_CR_ADDIS	(1<<1)		// ADC disable command
#define ADC_CR_ADEN	(1<<0)		// ADC enable command

/* p 592 */
#define ADC_CFGR_JQDIS		(1<<31)		// Injected Queue Disable
#define ADC_CFGR_AWD1CH		(0x1f<<26)
#define ADC_CFGR_JAUTO		(1<<25)
#define ADC_CFGR_JAWD1EN	(1<<24)
#define ADC_CFGR_AWD1EN		(1<<23)
#define ADC_CFGR_AWD1SGL	(1<<22)
#define ADC_CFGR_JQM		(1<<21)
#define ADC_CFGR_JDISCEN	(1<<20)
#define ADC_CFGR_DISCNUM	(0x7<<17)
#define ADC_CFGR_DISCEN		(1<<16)		// discontinuous mode
#define ADC_CFGR_AUTDLY		(1<<14)
#define ADC_CFGR_CONT		(1<<13)
#define ADC_CFGR_OVRMOD		(1<<12)
#define ADC_CFGR_EXTEN		(0x3<<10)
#define ADC_CFGR_EXTSEL3	(1<<9)
#define ADC_CFGR_EXTSEL2	(1<<8)
#define ADC_CFGR_EXTSEL1	(1<<7)
#define ADC_CFGR_EXTSEL0	(1<<6)
#define ADC_CFGR_ALIGN		(1<<5)
#define ADC_CFGR_RES		(0x3<<3)
#define ADC_CFGR_DFSDMCFG	(1<<2)
#define ADC_CFGR_DMACFG		(1<<1)
#define ADC_CFGR_DMAEN		(1<<0)

/* p598 */
#define ADC_SMPR1_SMP9		(0x7<<27)
#define ADC_SMPR1_SMP9_0	(1<<29)
#define ADC_SMPR1_SMP9_1	(1<<28)
#define ADC_SMPR1_SMP9_2	(1<<27)

#define ADC_SMPR1_SMP8		(0x7<<24)
#define ADC_SMPR1_SMP8_0	(1<<26)
#define ADC_SMPR1_SMP8_1	(1<<25)
#define ADC_SMPR1_SMP8_2	(1<<24)

#define ADC_SMPR1_SMP7		(0x7<<21)
#define ADC_SMPR1_SMP7_0	(1<<23)
#define ADC_SMPR1_SMP7_1	(1<<22)
#define ADC_SMPR1_SMP7_2	(1<<21)

#define ADC_SMPR1_SMP6		(0x7<<18)
#define ADC_SMPR1_SMP6_0	(1<<20)
#define ADC_SMPR1_SMP6_1	(1<<19)
#define ADC_SMPR1_SMP6_2	(1<<18)

#define ADC_SMPR1_SMP5		(0x7<<15)
#define ADC_SMPR1_SMP5_0	(1<<17)
#define ADC_SMPR1_SMP5_1	(1<<16)
#define ADC_SMPR1_SMP5_2	(1<<15)

#define ADC_SMPR1_SMP4		(0x7<<12)
#define ADC_SMPR1_SMP4_0	(1<<14)
#define ADC_SMPR1_SMP4_1	(1<<13)
#define ADC_SMPR1_SMP4_2	(1<<12)

#define ADC_SMPR1_SMP3		(0x7<<9)
#define ADC_SMPR1_SMP3_0	(1<<11)
#define ADC_SMPR1_SMP3_1	(1<<10)
#define ADC_SMPR1_SMP3_2	(1<<9)

#define ADC_SMPR1_SMP2		(0x7<<6)
#define ADC_SMPR1_SMP2_0	(1<<8)
#define ADC_SMPR1_SMP2_1	(1<<7)
#define ADC_SMPR1_SMP2_2	(1<<6)

#define ADC_SMPR1_SMP1		(0x7<<3)
#define ADC_SMPR1_SMP1_0	(1<<5)
#define ADC_SMPR1_SMP1_1	(1<<4)
#define ADC_SMPR1_SMP1_2	(1<<3)

#define ADC_SMPR1_SMP0		(0x7<<0)
#define ADC_SMPR1_SMP0_0	(1<<2)
#define ADC_SMPR1_SMP0_1	(1<<1)
#define ADC_SMPR1_SMP0_2	(1<<0)

/* p602 */
#define ADC_SQR1_SQ4	(0x1f<<24)
#define ADC_SQR1_SQ3	(0x1f<<18)
#define ADC_SQR1_SQ2	(0x1f<<12)
#define ADC_SQR1_SQ1	(0x1f<<6)
#define ADC_SQR1_L	(0xf<<0)

/* p610 */
#define ADC_DIFSEL_DIFSEL	(0x7ffff)
#define ADC_DIFSEL_DIFSEL_0	(1<<0)
#define ADC_DIFSEL_DIFSEL_1	(1<<1)
#define ADC_DIFSEL_DIFSEL_2	(1<<2)
#define ADC_DIFSEL_DIFSEL_3	(1<<3)
#define ADC_DIFSEL_DIFSEL_4	(1<<4)
#define ADC_DIFSEL_DIFSEL_5	(1<<5)
#define ADC_DIFSEL_DIFSEL_6	(1<<6)
#define ADC_DIFSEL_DIFSEL_7	(1<<7)
#define ADC_DIFSEL_DIFSEL_8	(1<<8)


typedef struct {
__IO uint32_t CSR;	/* 0x00 = Common Status Register */
__IO uint32_t RESERVED1;/* 0x04 = Common Status Register */
__IO uint32_t CCR;	/* 0x08 = Common Control Register */
__IO uint32_t CDR;	/* 0x0c = Regular Data Register */
} ADC_COMMON_TypeDef;

#define ADC123_COMMON_BASE	(AHB2PERIPH_BASE + 0x8040300)
#define ADC123_COMMON ((ADC_COMMON_TypeDef *)ADC123_COMMON_BASE)

/* p 611 */
#define ADC_CSR_JQOVF_SLV	(1<<26)
#define ADC_CSR_AWD3_SLV	(1<<25)
#define ADC_CSR_AWD2_SLV	(1<<24)
#define ADC_CSR_AWD1_SLV	(1<<23)
#define ADC_CSR_JEOS_SLV	(1<<22)
#define ADC_CSR_JEOC_SLV	(1<<21)
#define ADC_CSR_OVR_SLV		(1<<20)
#define ADC_CSR_EOS_SLV		(1<<19)
#define ADC_CSR_EOC_SLV		(1<<18)
#define ADC_CSR_EOSMP_SLV	(1<<17)
#define ADC_CSR_ADRDY_SLV	(1<<16)

#define ADC_CSR_JQOVF_MST	(1<<10)
#define ADC_CSR_AWD3_MST	(1<<9)
#define ADC_CSR_AWD2_MST	(1<<8)
#define ADC_CSR_AWD1_MST	(1<<7)
#define ADC_CSR_JEOS_MST	(1<<6)
#define ADC_CSR_JEOC_MST	(1<<5)
#define ADC_CSR_OVR_MST		(1<<4)
#define ADC_CSR_EOS_MST		(1<<3)
#define ADC_CSR_EOC_MST		(1<<2)
#define ADC_CSR_EOSMP_MST	(1<<1)
#define ADC_CSR_ADRDY_MST	(1<<0)


#define ADC_CCR_CH18SEL	(1<<24)
#define ADC_CCR_CH17SEL	(1<<23)
#define ADC_CCR_VREFEN	(1<<22)
#define ADC_CCR_PRESC	(0xf<<18)
#define ADC_CCR_CKMODE	(0x3<<16)
#define ADC_CCR_CKMODE_1	(0x1<<17)
#define ADC_CCR_CKMODE_0	(0x1<<16)
#define ADC_CCR_MDMA	(0x3<<14)
#define ADC_CCR_DMACFG	(0x3<<13)
#define ADC_CCR_DELAY	(0xf<<8)
#define ADC_CCR_DUAL	(0x1f<<0)


/* DAC -- Chapter 19 starting p620 */
/*	register definitions start in 19.7 on p638 */
typedef struct {
__IO uint32_t CR;	/* 0x00 = Control Register */
__IO uint32_t SWTRIGR;	/* 0x04 = Software Trigger Register */
			/* GRRR in the docs this should be SWTRGR */
			/* the textbook/keil stuff has it wrong */
__IO uint32_t DHR12R1;	/* 0x08 = CH1 12-bit right-aligned data register */
__IO uint32_t DHR12L1;	/* 0x0C = CH1 12-bit left-aligned data register */
__IO uint32_t DHR8R1;	/* 0x10 = CH1 8-bit right-aligned data register */
__IO uint32_t DHR12R2;	/* 0x14 = CH2 12-bit right-aligned data register */
__IO uint32_t DHR12L2;	/* 0x18 = CH2 12-bit left-aligned data register */
__IO uint32_t DHR8R2;	/* 0x1C = CH2 8-bit right-aligned data register */
__IO uint32_t DHR12RD;	/* 0x20 = Dual 12-bit right-aligned data register */
__IO uint32_t DHR12LD;	/* 0x24 = Dual 12-bit left-aligned data register */
__IO uint32_t DHR8RD;	/* 0x28 = Dual 8-bit right-aligned data register */
__IO uint32_t DOR1;	/* 0x2C = CH1 data output register */
__IO uint32_t DOR2;	/* 0x30 = CH2 data output register */
__IO uint32_t SR;	/* 0x34 = Status register */
__IO uint32_t CCR;	/* 0x38 = Calibration Control register */
__IO uint32_t MCR;	/* 0x3C = Mode Control register */
__IO uint32_t SHSR1;	/* 0x40 = CH1 sample and hold */
__IO uint32_t SHSR2;	/* 0x44 = CH2 sample and hold */
__IO uint32_t SHHR;	/* 0x48 = Sample and hold time */
__IO uint32_t SHRR;	/* 0x4C = Sample and hold refresh time */
} DAC_TypeDef;

/* p80 */
#define DAC_BASE		(APB1PERIPH_BASE + 0x7400)
#define DAC ((DAC_TypeDef *)DAC_BASE)

/* p639 */
#define DAC_CR_CEN2		(1<<30)
#define DAC_CR_DMAUDRIE2	(1<<29)
#define DAC_CR_DMAEN2		(1<<28)
#define DAC_CR_MAMP2		(0xf<<24)
#define DAC_CR_WAVE2		(0x3<<22)
#define DAC_CR_TSEL2		(0x7<<19)

#define DAC_CR_TSEL2_2		(1<<21)
#define DAC_CR_TSEL2_1		(1<<20)
#define DAC_CR_TSEL2_0		(1<<19)

#define DAC_CR_TEN2		(1<<18)
#define DAC_CR_EN2		(1<<16)

#define DAC_CR_CEN1		(1<<14)
#define DAC_CR_DMAUDRIE1	(1<<13)
#define DAC_CR_DMAEN1		(1<<12)
#define DAC_CR_MAMP1		(0xf<<8)
#define DAC_CR_WAVE1		(0x3<<6)
#define DAC_CR_TSEL1		(0x7<<3)

#define DAC_CR_TSEL1_2		(1<<5)
#define DAC_CR_TSEL1_1		(1<<4)
#define DAC_CR_TSEL1_0		(1<<3)

#define DAC_CR_TEN1		(1<<2)
#define DAC_CR_EN1		(1<<0)

/* p641 */
#define DAC_SWTRIGR_SWTRIG2	(1<<1)
#define DAC_SWTRIGR_SWTRIG1	(1<<0)

/* p647 */
#define DAC_SR_BWST2		(1<<31)	// Channel 2 busy writing
#define DAC_SR_CAL_FLAG2	(1<<30) // Channel 2 calibrate
#define DAC_SR_DMAUDR2		(1<<29) // Channel 2 DMA underwrite
#define DAC_SR_BWST1		(1<<15)	// Channel 2 busy writing
#define DAC_SR_CAL_FLAG1	(1<<14) // Channel 2 calibrate
#define DAC_SR_DMAUDR1		(1<<13) // Channel 2 DMA underwrite

/* p648 */
#define DAC_MCR_MODE2		(0x7<<16)
#define DAC_MCR_MODE1		(0x7<<0)


/* OPAMP -- Chapter 23 starting p695 */
/*	register definitions start in 23.5 on 703 */
typedef struct {
__IO uint32_t CSR;	/* 0x00 = Control/Status Register */
__IO uint32_t OTR;	/* 0x04 = Offset Trimming Register */
__IO uint32_t LPOTR;	/* 0x08 = Low-power Offset Trimming Register */
} OPAMP_TypeDef;

#define OPAMP1_BASE		(APB1PERIPH_BASE + 0x7800)
#define OPAMP2_BASE		(APB1PERIPH_BASE + 0x7810)
#define OPAMP1 ((OPAMP_TypeDef *)OPAMP1_BASE)
#define OPAMP2 ((OPAMP_TypeDef *)OPAMP2_BASE)



/* RTC -- Chapter 38.6 (p1238)*/
typedef struct {
__IO uint32_t TR;	/* 0x00 = Time Register */
__IO uint32_t DR;	/* 0x04 = Date Register */
__IO uint32_t CR;	/* 0x08 = Control Register */
__IO uint32_t ISR;	/* 0x0C = Init and Status Register */
__IO uint32_t PRER;	/* 0x10 = Prescalar Register */
__IO uint32_t WUTR;	/* 0x14 = Wakeup Time Register */
__IO uint32_t RESERVED0;/* 0x18 */
__IO uint32_t ALRMAR;	/* 0x1C = Alarm A register */
__IO uint32_t ALRMBR;	/* 0x20 = Alarm B register */
__IO uint32_t WPR;	/* 0x24 = Write Protect register */
} RTC_TypeDef;

#define RTC_BASE	(APB1PERIPH_BASE + 0x2800)
#define RTC ((RTC_TypeDef *)RTC_BASE)

/* LCD -- Chapter 25.6 (p767,p790) */
typedef struct {
__IO uint32_t CR;		/* 0x00 = Control Register */
__IO uint32_t FCR;		/* 0x04 = Frame Control Register */
__IO uint32_t SR;		/* 0x08 = Status Register */
__IO uint32_t CLR;		/* 0x0C = Clear Register */
__IO uint32_t RESERVED0;	/* 0x10 = Unused */
__IO uint32_t RAM[16];		/* 0x14 - 0x50 */
} LCD_TypeDef;

#define LCD_BASE	(APB1PERIPH_BASE + 0x2400)
#define LCD ((LCD_TypeDef *)LCD_BASE)

/* p790 */
#define LCD_CR_LCDEN		(0x1<<0)
#define LCD_CR_VSEL		(0x1<<1)
#define LCD_CR_DUTY_MASK	(0x7<<2)
#define LCD_CR_DUTY_1_4		(0x3<<2)
#define LCD_CR_BIAS_MASK	(0x3<<5)
#define LCD_CR_BIAS_1_3		(0x2<<5)
#define LCD_CR_MUX_SEG		(0x1<<7)


/* p791 */
#define LCD_FCR_PON_MASK	(0x7<<4)
#define LCD_FCR_PON7		(0x7<<4)
#define LCD_FCR_CC_MASK		(0x7<<10)
#define LCD_FCR_CC_LCD7		(0x7<<10)

/* p794 */
#define LCD_SR_ENS		(0x1<<0)
#define LCD_SR_UDR		(0x1<<2)
#define LCD_SR_UDD		(0x1<<3)
#define LCD_SR_RDY		(0x1<<4)
#define LCD_SR_FCRSF		(0x1<<5)




/* Power -- 5.4 p184 */
typedef struct {
__IO uint32_t CR1;	/* 0x00 = Control Register 1 */
__IO uint32_t CR2;	/* 0x04 = Control Register 2 */
__IO uint32_t CR3;	/* 0x08 = Control Register 3 */
__IO uint32_t CR4;	/* 0x0C = Control Register 4 */
} PWR_TypeDef;

/* Table 1 p80 */
#define PWR_BASE	(APB1PERIPH_BASE + 0x7000)
#define PWR ((PWR_TypeDef *)PWR_BASE)

#define PWR_CR1_DBP	(1<<8)	/* Disable Backup Write Protection */


/************************************************************/
/************************************************************/
/* Cortex-M4 stuff, see the
	Cortex-M4 Devices Generic User Guide DUI0553.pdf    */
/************************************************************/
/************************************************************/

typedef struct {
__IO uint32_t CTRL;	/* Control / Status register */
__IO uint32_t LOAD;	/* Reload value register */
__IO uint32_t VAL;	/* Current value register */
__I  uint32_t CALIB;	/* Calibration register */
} SysTick_TypeDef;

#define SysTick_BASE	0xE000E010
#define SysTick		((SysTick_TypeDef *)SysTick_BASE)

#define SysTick_CTRL_ENABLE_Msk		0x1
#define SysTick_CTRL_TICKINT_Msk	0x2
#define SysTick_CTRL_CLKSOURCE_Msk	0x4


/* STM32L476xx interrupt numbers */
/* See Table 57 on page 396 of manual */
#define TIM1_CC_IRQn		27	// tim1 capture/compare interrupt
#define TIM2_IRQn		28	// tim2 global interrupt
#define TIM3_IRQn		29	// tim3 global interrupt
#define TIM4_IRQn		30	// tim4 global interrupt

/* Cortex-M4 Processor Exceptions Numbers */
#define NonMaskableInt_IRQn	-14	// Non Maskable Interrupt
#define	HardFault_IRQn		-13	// Memory Management Interrupt
#define	MemoryManagement_IRQn	-12	// Memory Management Interrupt
#define	BusFault_IRQn		-11	// Bus Fault Interrupt
#define UsageFault_IRQn		-10	// Usage Fault Interrupt
#define	SVCall_IRQn		-5	// SV Call Interrupt
#define	DebugMonitor_IRQn	-4	// Debug Monitor Interrupt
#define	PendSV_IRQn		-2	// Pend SV Interrupt
#define	SysTick_IRQn		-1	// System Tick Interrupt

#define	__NVIC_PRIO_BITS	4       // STM32L4XX uses 4 Bits for
					// the interrupt Priority Levels


typedef struct {
__IO uint32_t 	ISER[8];	// 0x000 (R/W) Interrupt Set Enable Register
     uint32_t 	RESERVED0[24];	// Reserved
__IO uint32_t	ICER[8];	// 0x080 (R/W) Interrupt Clear Enable Register
     uint32_t 	RSERVED1[24];	// Reserved
__IO uint32_t	ISPR[8];	// 0x100 (R/W) Interrupt Set Pending Register
     uint32_t 	RESERVED2[24];	// Reserved
__IO uint32_t	ICPR[8];	// 0x180 (R/W) Interrupt Clear Pending Register
     uint32_t	RESERVED3[24];	// Reserved
__IO uint32_t   IABR[8];	// 0x200 (R/W) Interrupt Active bit Register
     uint32_t	RESERVED4[56];	// Reserved
__IO uint8_t	IP[240];	// 0x300 (R/W) Interrupt Priority Register (8Bit wide)
     uint32_t 	RESERVED5[644];	// Reserved
__O  uint32_t	STIR;		// Offset: 0xE00 ( /W) Software Trigger Interrupt Register
} NVIC_TypeDef;


#define NVIC_BASE	0xE000E100
#define NVIC		((NVIC_TypeDef *)NVIC_BASE)



typedef struct {
     uint32_t	RESERVED1[2];	// Reserved
__IO uint32_t	ACTLR;		// 0xE000E008 Auxiliary Control Register
     uint32_t   RESERVED2[0xd00-12];
__I  uint32_t	CPUID;		// 0xE000ED00 CPUID
__IO uint32_t	ICSR;		// 0xE000ED04 Interrupt Control and State Register
__IO uint32_t	VTOR;		// 0xE000ED08 Vector Table Offset Register
__IO uint32_t	AIRCR;		// 0xE000ED0C Application Interrupt and Reset Control Register
__IO uint32_t	SCR;		// 0xE000ED10 System Control Register
__IO uint32_t	CCR;		// 0xE000ED14 Configuration and Control Register
__IO uint8_t	SHP[12];	// 0xE000ED18 System Handler Priority Register
__IO uint32_t   SHCRS;		// 0xE000ED24 System Handler Control and State Register
/* ... there's more */
} SCB_TypeDef;

#define SCB_BASE	0xE000E000
#define SCB		((SCB_TypeDef *)SCB_BASE)




/* Advanced timers: TIM1/TIM8 */
/* Chapter 30, page 907 */

typedef struct {
__IO uint32_t	CR1;		// 0x00 TIM1 Control Register
__IO uint32_t	CR2;		// 0x04 TIM1 Control Register 2
__IO uint32_t	SMCR;		// 0x08 TIM1 Slave mode control register
__IO uint32_t	DIER;		// 0x0C TIM1 DMA/interrupt enable register
__IO uint32_t	SR;		// 0x10 TIM1 Status Register
__IO uint32_t	EGR;		// 0x14 TIM1 Event Generation Register
__IO uint32_t	CCMR1;		// 0x18 TIM1 Capture/Compare Mode R1
__IO uint32_t	CCMR2;		// 0x1C TIM1 Capture/Compare Mode R2
__IO uint32_t	CCER;		// 0x20 TIM1 Capture/Compare Enable Register
__IO uint32_t	CNT;		// 0x24 TIM1 Counter Register
__IO uint32_t	PSC;		// 0x28 TIM1 Prescaler
__IO uint32_t	ARR;		// 0x2C TIM1 Auto-reload
__IO uint32_t	RCR;		// 0x30 TIM1 Reptition Count
__IO uint32_t	CCR1;		// 0x34 TIM1 Capture/Compare R1
__IO uint32_t	CCR2;		// 0x38 TIM1 Capture/Compare R2
__IO uint32_t	CCR3;		// 0x3C TIM1 Capture/Compare R3
__IO uint32_t	CCR4;		// 0x40 TIM1 Capture/Compare R4
__IO uint32_t	BDTR;		// 0x44 TIM1 Break and Dead Time register
__IO uint32_t	DCR;		// 0x48 TIM1 DMA Control Register
__IO uint32_t	DMAR;		// 0x4C TIM1 DMA Address
__IO uint32_t	OR1;		// 0x50 TIM1 Option Register 1
__IO uint32_t	CCMR3;		// 0x54 TIM1 Capure/Compare Mode R3
__IO uint32_t	CCR5;		// 0x58 TIM1 Capture/Compare R5
__IO uint32_t	CCR6;		// 0x5C TIM1 Capture/Compare R6
__IO uint32_t	OR2;		// 0x60 TIM1 Option Register 2
__IO uint32_t	OR3;		// 0x64 TIM1 Option Register 3

} TIM1_TypeDef;

// p 79
#define TIM1_BASE	(APB2PERIPH_BASE + 0x2C00)
#define TIM1		((TIM1_TypeDef *)TIM1_BASE)

#define TIM8_BASE	(APB2PERIPH_BASE + 0x3400)
#define TIM8		((TIM1_TypeDef *)TIM8_BASE)

// p???? for TIM1/TIM8
// p1058 for TIM2/3/4/5
#define TIM_CR1_CEN	(1<<0)		// Counter Enable
#define TIM_CR1_UDIS	(1<<1)		// Update disable
#define TIM_CR1_URS	(1<<2)		// Update request source
#define TIM_CR1_OPM	(1<<3)		// One pulse mode
#define TIM_CR1_DIR	(1<<4)		// Direction
#define TIM_CR1_CMS	(3<<5)		// Center aligned mode
#define TIM_CR1_CMS_0	(1<<5)		// Center aligned mode
#define TIM_CR1_CMS_1	(1<<6)		// Center aligned mode
#define TIM_CR1_ARPE	(1<<7)		// Auto-reload-preload enable
#define TIM_CR1_CKD	(3<<8)		// Clock division
#define TIM_CR1_UIFREMAP (1<<11)	// UIF status bit remapping

/* p1060 */
#define TIM_CR2_TIS	(1<<7)
#define TIM_CR2_MMS	(0x7<<4)	// Master Mode Selection
#define TIM_CR2_MMS_2	(1<<6)
#define TIM_CR2_MMS_1	(1<<5)
#define TIM_CR2_MMS_0	(1<<4)
#define TIM_CR2_CCDS	(1<<3)		// capture/compare DMA

// p1064 for TIM2/3/4/5
#define TIM_DIER_TDE		(1<<14) // Trigger DMA request enable
#define TIM_DIER_CC4DE		(1<<12) // capture/compare4 dma request enable
#define TIM_DIER_CC3DE		(1<<11) // capture/compare3 dma request enable
#define TIM_DIER_CC2DE		(1<<10) // capture/compare2 dma request enable
#define TIM_DIER_CC1DE		(1<<9)  // capture/compare1 dma request enable
#define TIM_DIER_UDE		(1<<8)  // updae dma request enable
#define TIM_DIER_TIE		(1<<6)  // trigger interrupt enable
#define TIM_DIER_CC4IE		(1<<4)  // capture/compare4 interrupt enable
#define TIM_DIER_CC3IE		(1<<3)  // capture/compare3 interrupt enable
#define TIM_DIER_CC2IE		(1<<2)  // capture/compare2 interrupt enable
#define TIM_DIER_CC1IE		(1<<1)  // capture/compare1 interrupt enable
#define TIM_DIER_UIE		(1<<0)  // update interrupt enable

// p1065 for TIM2/3/4/5 */

#define TIM_SR_CC4OF	(1<<12)	// capture/compare4 overcapture
#define TIM_SR_CC3OF	(1<<11)	// capture/compare3 overcapture
#define TIM_SR_CC2OF	(1<<10)	// capture/compare2 overcapture
#define TIM_SR_CC1OF	(1<<9)	// capture/compare1 overcapture
#define TIM_SR_TIF	(1<<6)	// trigger interrupt flag
#define TIM_SR_CC4IF	(1<<4)  // capture/compare 4 interrupt flag
#define TIM_SR_CC3IF	(1<<3)  // capture/compare 3 interrupt flag
#define TIM_SR_CC2IF	(1<<2)  // capture/compare 2 interrupt flag
#define TIM_SR_CC1IF	(1<<1)  // capture/compare 1 interrupt flag
#define TIM_SR_UIF	(1<<0)	// update interrupt flag

// p1067 for TIM2/3/4/5

/* Bits when in output capture mode */

// masks
#define TIM_CCMR1_OC1M	((1<<16) | (1<<6) | (1<<5) | (1<<4))
#define TIM_CCMR1_OC2M	((1<<24) | (1<<14) | (1<<13) | (1<<12))
#define TIM_CCMR1_CC2S	((1<<9)|(1<<8))
#define TIM_CCMR1_CC1S	((1<<1)|(1<<0))

// pins
#define TIM_CCMR1_OC2M_3	(1<<24)
#define TIM_CCMR1_OC1M_3	(1<<16)
#define TIM_CCMR1_OC2CE		(1<<15) // output compare 2 clear enable
#define TIM_CCMR1_OC2M_2	(1<<14)
#define TIM_CCMR1_OC2M_1	(1<<13)
#define TIM_CCMR1_OC2M_0	(1<<12)
#define TIM_CCMR1_OC2PE		(1<<11) // output compare 2 preload enable
#define TIM_CCMR1_OC2FE		(1<<10) // output compare 2 fast enable
#define TIM_CCMR1_CC2S_1	(1<<9)
#define TIM_CCMR1_CC2S_0	(1<<8)
#define TIM_CCMR1_OC1CE		(1<<7) // output compare 1 clear enable
#define TIM_CCMR1_OC1M_2	(1<<6)
#define TIM_CCMR1_OC1M_1	(1<<5)
#define TIM_CCMR1_OC1M_0	(1<<4)
#define TIM_CCMR1_OC1PE		(1<<3)	// preload enable
#define TIM_CCMR1_OC1FE		(1<<2)  // output compare 1 fast enable
#define TIM_CCMR1_CC1S_1	(1<<1)
#define TIM_CCMR1_CC1S_0	(1<<0)

/* bits when in input capture mode */
#define TIM_CCMR1_IC2F		((1<<15) | (1<<14) | (1<<13) | (1<<12))
#define TIM_CCMR1_IC2PSC	((1<<11)|(1<<10))
#define TIM_CCMR1_CC2S		((1<<9)|(1<<8))
#define TIM_CCMR1_IC1F		((1<<7) | (1<<6) | (1<<5) | (1<<4))
#define TIM_CCMR1_IC1PSC	((1<<3)|(1<<2))
#define TIM_CCMR1_CC1S		((1<<1)|(1<<0))

#define TIM_CCMR1_IC2F_3	(1<<15)
#define TIM_CCMR1_IC2F_2	(1<<14)
#define TIM_CCMR1_IC2F_1	(1<<13)
#define TIM_CCMR1_IC2F_0	(1<<12)
#define TIM_CCMR1_IC2PSC_1	(1<<11)
#define TIM_CCMR1_IC2PSC_0	(1<<10)
#define TIM_CCMR1_CC2S_1	(1<<9)
#define TIM_CCMR1_CC2S_0	(1<<8)
#define TIM_CCMR1_IC1F_3	(1<<7)
#define TIM_CCMR1_IC1F_2	(1<<6)
#define TIM_CCMR1_IC1F_1	(1<<5)
#define TIM_CCMR1_IC1F_0	(1<<4)
#define TIM_CCMR1_IC1PSC_1	(1<<3)
#define TIM_CCMR1_IC1PSC_0	(1<<2)
#define TIM_CCMR1_CC1S_1	(1<<1)
#define TIM_CCMR1_CC1S_0	(1<<0)




// p1073 TIM2/3/4/5
#define TIM_CCER_CC1E		(1<<0)	// enable
#define TIM_CCER_CC1P		(1<<1)	// polarity
#define TIM_CCER_CC1NE		(1<<2)	// complement enable
#define TIM_CCER_CC1NP		(1<<3)	// complement polarity
#define TIM_CCER_CC2E		(1<<4)	// enable
#define TIM_CCER_CC2P		(1<<5)	// polarity
#define TIM_CCER_CC2NE		(1<<6)	// complement enable
#define TIM_CCER_CC2NP		(1<<7)	// complement polarity
#define TIM_CCER_CC3E		(1<<8)	// enable
#define TIM_CCER_CC3P		(1<<9)	// polarity
#define TIM_CCER_CC3NE		(1<<10)	// complement enable
#define TIM_CCER_CC3NP		(1<<11)	// complement polarity
#define TIM_CCER_CC4E		(1<<12)	// enable
#define TIM_CCER_CC4P		(1<<13)	// polarity
//#define TIM_CCER_CC4NE	RESERVED	// complement enable
#define TIM_CCER_CC4NP		(1<<15)	// complement polarity
#define TIM_CCER_CC5E		(1<<16)	// enable
#define TIM_CCER_CC5P		(1<<17)	// polarity
//#define TIM_CCER_CC5NE	RESERVED	// complement enable
//#define TIM_CCER_CC5NP	RESERVED	// complement polarity
#define TIM_CCER_CC6E		(1<<20)	// enable
#define TIM_CCER_CC6P		(1<<21)	// polarity
//#define TIM_CCER_CC6NE	RESERVED	// complement enable
//#define TIM_CCER_CC6NP	RESERVED	// complement polarity


#define TIM_BDTR_AOE	(1<<14) // automatic output enable
#define TIM_BDTR_MOE	(1<<15)	// main output enable

/* Advanced timers: TIM2/TIM3/TIM4/TIM5 */
/* Chapter 31, page 1013 */
/* TIM3/TIM4 are 16bit, TIM2/TIM5 are 32 bit */
/* Registers start Chapter 31.4, p1058 */
typedef struct {
__IO uint32_t	CR1;		// 0x00 TIM2 Control Register
__IO uint32_t	CR2;		// 0x04 TIM2 Control Register 2
__IO uint32_t	SMCR;		// 0x08 TIM2 Slave mode control register
__IO uint32_t	DIER;		// 0x0C TIM2 DMA/interrupt enable register
__IO uint32_t	SR;		// 0x10 TIM2 Status Register
__IO uint32_t	EGR;		// 0x14 TIM2 Event Generation Register
__IO uint32_t	CCMR1;		// 0x18 TIM2 Capture/Compare Mode R1
__IO uint32_t	CCMR2;		// 0x1C TIM2 Capture/Compare Mode R2
__IO uint32_t	CCER;		// 0x20 TIM2 Capture/Compare Enable Register
__IO uint32_t	CNT;		// 0x24 TIM2 Counter Register
__IO uint32_t	PSC;		// 0x28 TIM2 Prescaler
__IO uint32_t	ARR;		// 0x2C TIM2 Auto-reload
__IO uint32_t	RESERVED1;	// 0x30 TIM2 ???
__IO uint32_t	CCR1;		// 0x34 TIM2 Capture/Compare R1
__IO uint32_t	CCR2;		// 0x38 TIM2 Capture/Compare R2
__IO uint32_t	CCR3;		// 0x3C TIM2 Capture/Compare R3
__IO uint32_t	CCR4;		// 0x40 TIM2 Capture/Compare R4
__IO uint32_t	RESERVED2;	// 0x44 TIM2 ???
__IO uint32_t	DCR;		// 0x48 TIM2 DMA Control Register
__IO uint32_t	DMAR;		// 0x4C TIM2 DMA Address
__IO uint32_t	OR1;		// 0x50 TIM2 Option Register 1
__IO uint32_t	CCMR3;		// 0x54 TIM2 Capure/Compare Mode R3
__IO uint32_t	RESERVED3;	// 0x58 TIM2
__IO uint32_t	RESERVED4;	// 0x5C TIM2
__IO uint32_t	OR2;		// 0x60 TIM2 Option Register 2
} TIM2_TypeDef;

// p 81
#define TIM2_BASE	(APB1PERIPH_BASE + 0)
#define TIM2		((TIM2_TypeDef *)TIM2_BASE)

#define TIM3_BASE	(APB1PERIPH_BASE + 0x400)
#define TIM3		((TIM2_TypeDef *)TIM3_BASE)

#define TIM4_BASE	(APB1PERIPH_BASE + 0x800)
#define TIM4		((TIM2_TypeDef *)TIM4_BASE)

#define TIM5_BASE	(APB1PERIPH_BASE + 0xc00)
#define TIM5		((TIM2_TypeDef *)TIM5_BASE)





/* i2c */
/* Chapter 39, page ??? */
/* Registers start Chapter 39.7, p1316 */
typedef struct {
__IO uint32_t	CR1;		// 0x00 I2C Control Register
__IO uint32_t	CR2;		// 0x04 I2C Control Register 2
__IO uint32_t	OAR1;		// 0x08 I2C Own Address 1 Register
__IO uint32_t	OAR2;		// 0x0C I2C Own Address 2 Register
__IO uint32_t	TIMINGR;	// 0x10 I2C Timing Register
__IO uint32_t	TIMEOUTR;	// 0x14 I2C Timeout Register
__IO uint32_t	ISR;		// 0x18 I2C Interrupt and Status Register
__IO uint32_t	ICR;		// 0x1C I2C Interrupt Clear Register
__IO uint32_t	PECR;		// 0x20 I2C PEC Register
__IO uint32_t	RXDR;		// 0x24 I2C Receive Data Register
__IO uint32_t	TXDR;		// 0x28 I2C Transmit Data Register
} I2C_TypeDef;

// p 80
#define I2C1_BASE	(APB1PERIPH_BASE + 0x5400)
#define I2C1		((I2C_TypeDef *)I2C1_BASE)

#define I2C2_BASE	(APB1PERIPH_BASE + 0x5800)
#define I2C2		((I2C_TypeDef *)I2C1_BASE)

#define I2C3_BASE	(APB1PERIPH_BASE + 0x5C00)
#define I2C3		((I2C_TypeDef *)I2C1_BASE)

/* p1316 */
#define I2C_CR1_PECEN		(1<<23)
#define I2C_CR1_ALERTEN		(1<<22)
#define I2C_CR1_SMBDEN		(1<<21)
#define I2C_CR1_SMBHEN		(1<<20)
#define I2C_CR1_GCEN		(1<<19)
#define I2C_CR1_WUPEN		(1<<18)
#define I2C_CR1_NOSTRETCH	(1<<17)
#define I2C_CR1_SBC		(1<<16)
#define I2C_CR1_RXDMAEN		(1<<15)
#define I2C_CR1_TXDMAEN		(1<<14)
#define I2C_CR1_ANFOFF		(1<<12)
#define I2C_CR1_DNF		(0xf<<8)
#define I2C_CR1_ERRIE		(1<<7)
#define I2C_CR1_TCIE		(1<<6)
#define I2C_CR1_STOPIE		(1<<5)
#define I2C_CR1_NACKIE		(1<<4)
#define I2C_CR1_ADDRIE		(1<<3)
#define I2C_CR1_RXIE		(1<<2)
#define I2C_CR1_TXIE		(1<<1)
#define I2C_CR1_PE		(1<<0)

/* p1319 */
#define I2C_CR2_PECBYTE		(1<<26)
#define I2C_CR2_AUTOEND		(1<<25)
#define I2C_CR2_RELOAD		(1<<24)
#define I2C_CR2_NBYTES		(0xff<<16)
#define I2C_CR2_NACK		(1<<15)
#define I2C_CR2_STOP		(1<<14)
#define I2C_CR2_START		(1<<13)
#define I2C_CR2_HEAD10R		(1<<12)
#define I2C_CR2_ADD10		(1<<11)
#define I2C_CR2_RD_WRN		(1<<10)
#define I2C_CR2_SADD		(0x3ff<<0)


/* p1322 */
#define I2C_OAR1_OA1EN		(1<<15)
#define I2C_OAR1_OA1MODE	(1<<10)
#define I2C_OAR1_OA1		(0x7f<<0)

/* p1323 */
#define I2C_OAR2_OA2EN		(1<<15)
#define I2C_OAR2_OA2MODE	(1<<10)
#define I2C_OAR2_OA2MSK		(0x7<<8)
#define I2C_OAR2_OA2		(0x7f<<1)

/* p1324 */
#define I2C_TIMINGR_PRESC	(0xf<<28)
#define I2C_TIMINGR_SCLDEL	(0xf<<20)
#define I2C_TIMINGR_SDADEL	(0xf<<16)
#define I2C_TIMINGR_SCLH	(0xff<<8)
#define I2C_TIMINGR_SCLL	(0xff<<0)


