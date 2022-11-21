/*
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#define XOSC_MHZ 12

#define KHZ 1000
#define MHZ 1000000

#define CLOCKS_BASE	0x40008000
#define PLL_SYS_BASE	0x40028000
#define PLL_USB_BASE	0x4002c000
#define WATCHDOG_BASE	0x40058000
#define XOSC_BASE	0x40024000

typedef struct {
    io_rw_32 ctrl;
    io_wo_32 load;
    io_ro_32 reason;
    io_rw_32 scratch[8];
    io_rw_32 tick;
} watchdog_hw_t;


#define watchdog_hw ((watchdog_hw_t *const)WATCHDOG_BASE)


enum clock_index {
    clk_gpout0 = 0,     ///< GPIO Muxing 0
    clk_gpout1,         ///< GPIO Muxing 1
    clk_gpout2,         ///< GPIO Muxing 2
    clk_gpout3,         ///< GPIO Muxing 3
    clk_ref,            ///< Watchdog and timers reference clock
    clk_sys,            ///< Processors, bus fabric, memory, memory mapped registers
    clk_peri,           ///< Peripheral clock for UART and SPI
    clk_usb,            ///< USB clock
    clk_adc,            ///< ADC clock
    clk_rtc,            ///< Real time clock
    CLK_COUNT
};

typedef struct {
    io_rw_32 ctrl;
    io_rw_32 div;
    io_rw_32 selected;
} clock_hw_t;

typedef struct {
    io_rw_32 ref_khz;
    io_rw_32 min_khz;
    io_rw_32 max_khz;
    io_rw_32 delay;
    io_rw_32 interval;
    io_rw_32 src;
    io_ro_32 status;
    io_ro_32 result;
} fc_hw_t;

typedef struct {
    clock_hw_t clk[CLK_COUNT];
    struct {
        io_rw_32 ctrl;
        io_rw_32 status;
    } resus;
    fc_hw_t fc0;
    io_rw_32 wake_en0;
    io_rw_32 wake_en1;
    io_rw_32 sleep_en0;
    io_rw_32 sleep_en1;
    io_rw_32 enabled0;
    io_rw_32 enabled1;
    io_rw_32 intr;
    io_rw_32 inte;
    io_rw_32 intf;
    io_rw_32 ints;
} clocks_hw_t;

#define clocks_hw ((clocks_hw_t *const)CLOCKS_BASE)


typedef struct {
    io_rw_32 cs;
    io_rw_32 pwr;
    io_rw_32 fbdiv_int;
    io_rw_32 prim;
} pll_hw_t;

#define pll_sys_hw ((pll_hw_t *const)PLL_SYS_BASE)
#define pll_usb_hw ((pll_hw_t *const)PLL_USB_BASE)

#define pll_sys pll_sys_hw
#define pll_usb pll_usb_hw

typedef pll_hw_t *PLL;

uint32_t clock_get_hz(enum clock_index clk_index);
