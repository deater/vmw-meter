/* In en.DM00083560.pdf:   */

#define __IO volatile

#define PERIPH_BASE	((uint32_t)0x40000000)
#define AHB1PERIPH_BASE	(PERIPH_BASE + 0x00020000)
#define AHB2PERIPH_BASE	(PERIPH_BASE + 0x08000000)

/* 6.4 p233 */
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
#define RCC_CR_MSIRGSEL		(1<<3)
#define RCC_CR_MSIRANGE		(0xf<<4)
#define RCC_CR_HSION		(1<<8)
#define RCC_CR_HSIRDY		(1<<10)
#define RCC_CR_HSERDY		(1<<17)
#define RCC_CR_HSEBYP		(1<<18)
#define RCC_CR_PLLON		(1<<24)
#define RCC_CR_PLLRDY		(1<<25)


#define RCC_CFGR_SW		(0x3)
#define RCC_CFGR_SW_PLL		(0x3)
#define RCC_CFGR_SW_HSI		(0x1)
#define RCC_CFGR_HPRE		(0xf<<4)

#define RCC_CFGR_SWS		(0x3<<2)
#define RCC_CFGR_SWS_PLL	(0x3<<2)

#define RCC_PLLCFGR_PLLSRC	(0x3)
#define RCC_PLLCFGR_PLLSRC_HSI	(0x2)
#define RCC_PLLCFGR_PLLN	(0xff<<8)
#define RCC_PLLCFGR_PLLM	(0x7<<4)
#define RCC_PLLCFGR_PLLR	(0x3<<25)
#define RCC_PLLCFGR_PLLREN	(1<<24)

#define RCC_AHB2ENR_GPIOAEN	(1<<0)
#define RCC_AHB2ENR_GPIOBEN	(1<<1)
#define RCC_AHB2ENR_GPIOCEN	(1<<2)
#define RCC_AHB2ENR_GPIODEN	(1<<3)
#define RCC_AHB2ENR_GPIOEEN	(1<<4)
#define RCC_AHB2ENR_GPIOFEN	(1<<5)
#define RCC_AHB2ENR_GPIOGEN	(1<<6)
#define RCC_AHB2ENR_GPIOHEN	(1<<7)
#define RCC_AHB2ENR_GPIOIEEN	(1<<8)

#define	RCC_CSR_MSISRANGE	(0xf<<8)


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

#define GPIOB_BASE	(AHB2PERIPH_BASE + 0x0400)

#define GPIOB ((GPIO_TypeDef *)GPIOB_BASE)

#define MODER_INPUT	0x00
#define MODER_OUTPUT	0x01
#define MODER_ALT	0x02
#define MODER_ANALOG	0x03

#define MODER_SET(_s,_v)	(((_v&0x3) << (_s*2))


#define FLASH_BASE	((uint32_t)0x08000000U)


#define RESET	0

