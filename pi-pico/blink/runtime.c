/*
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#define PICO_NO_RAM_VECTOR_TABLE 0
#define PICO_NO_FLASH	0

#include <stdio.h>
#include <stdarg.h>
//#include "pico.h"

//#include "hardware/regs/m0plus.h"
#include "include/resets.h"
//#include "hardware/structs/mpu.h"

#define PPB_BASE 0xe0000000
#define M0PLUS_MPU_TYPE_OFFSET	0x0000ed90
#define M0PLUS_CPUID_OFFSET	0x0000ed00
#define M0PLUS_NVIC_IPR0_OFFSET	0x0000e400

typedef volatile uint32_t io_rw_32;
typedef const volatile uint32_t io_ro_32;
typedef volatile uint32_t io_wo_32;

typedef struct {
    io_ro_32 type;
    io_rw_32 ctrl;
    io_rw_32 rnr;
    io_rw_32 rbar;
    io_rw_32 rasr;
} mpu_hw_t;

#define mpu_hw ((mpu_hw_t *const)(PPB_BASE + M0PLUS_MPU_TYPE_OFFSET))



//#include "hardware/structs/scb.h"

// SCB == System Control Block
typedef struct {
    io_ro_32 cpuid;
    io_rw_32 icsr;
    io_rw_32 vtor;
    io_rw_32 aircr;
    io_rw_32 scr;
    // ...
} armv6m_scb_t;

#define scb_hw ((armv6m_scb_t *const)(PPB_BASE + M0PLUS_CPUID_OFFSET))

//#include "hardware/structs/padsbank0.h"

#include "include/pads_bank0.h"

// from src/rp2040/hardware_regs/include/hardware/regs/addressmap.h
#define PADS_BANK0_BASE 0x4001c000

typedef struct {
    io_rw_32 voltage_select;
    io_rw_32 io[30];
} padsbank0_hw_t;

#define padsbank0_hw ((padsbank0_hw_t *)PADS_BANK0_BASE)


//#include "hardware/clocks.h"
void clocks_init(void);
//#include "hardware/irq.h"
//#include "hardware/resets.h"

//#include "pico/mutex.h"
//#include "pico/time.h"
//#include "pico/printf.h"

#if PICO_ENTER_USB_BOOT_ON_EXIT
//#include "pico/bootrom.h"
#endif

#ifndef PICO_NO_RAM_VECTOR_TABLE
#define PICO_NO_RAM_VECTOR_TABLE 0
#endif

extern char __StackLimit; /* Set by linker.  */

uint32_t __attribute__((section(".ram_vector_table"))) ram_vector_table[48];

// this is called for each thread since they have their own MPU
void runtime_install_stack_guard(void *stack_bottom) {
    // this is called b4 runtime_init is complete, so beware printf or assert

    // make sure no one is using the MPU yet
    if (mpu_hw->ctrl) {
        // Note that it would be tempting to change this to a panic, but it happens so early, printing is not a good idea
//        __breakpoint();
    }

    uintptr_t addr = (uintptr_t) stack_bottom;
    // the minimum we can protect is 32 bytes on a 32 byte boundary, so round up which will
    // just shorten the valid stack range a tad
    addr = (addr + 31u) & ~31u;

    // mask is 1 bit per 32 bytes of the 256 byte range... clear the bit for the segment we want
    uint32_t subregion_select = 0xffu ^ (1u << ((addr >> 5u) & 7u));
    mpu_hw->ctrl = 5; // enable mpu with background default map
    mpu_hw->rbar = (addr & (uint)~0xff) | 0x8 | 0;
    mpu_hw->rasr = 1 // enable region
                   | (0x7 << 1) // size 2^(7 + 1) = 256
                   | (subregion_select << 8)
                   | 0x10000000; // XN = disable instruction fetch; no other bits means no permissions
}


// ./src/rp2_common/hardware_base/include/hardware/address_mapped.h

#define REG_ALIAS_SET_BITS (0x2u << 12u)
#define REG_ALIAS_CLR_BITS (0x3u << 12u)

#define hw_set_alias_untyped(addr) ((void *)(REG_ALIAS_SET_BITS | (uintptr_t)(addr)))
#define hw_clear_alias_untyped(addr) ((void *)(REG_ALIAS_CLR_BITS | (uintptr_t)(addr)))
#define hw_clear_alias(p) ((typeof(p))hw_clear_alias_untyped(p))

static void hw_set_bits(io_rw_32 *addr, uint32_t mask) {
    *(io_rw_32 *) hw_set_alias_untyped((volatile void *) addr) = mask;
}

inline static void hw_clear_bits(io_rw_32 *addr, uint32_t mask) {
    *(io_rw_32 *) hw_clear_alias_untyped((volatile void *) addr) = mask;
}


// ./src/rp2040/hardware_structs/include/hardware/structs/resets.h

typedef struct {
    io_rw_32 reset;
    io_rw_32 wdsel;
    io_rw_32 reset_done;
} resets_hw_t;


#define RESETS_BASE 0x4000c000
#define resets_hw ((resets_hw_t *const)RESETS_BASE)


static void reset_block(uint32_t bits) {
    hw_set_bits(&resets_hw->reset, bits);
}

static void tight_loop_contents(void) {}

static void unreset_block_wait(uint32_t bits) {
    hw_clear_bits(&resets_hw->reset, bits);
    while (~resets_hw->reset_done & bits)
        tight_loop_contents();
}

#define PICO_DEFAULT_IRQ_PRIORITY 0x80
#define NUM_IRQS 32

void irq_set_priority(uint num, uint8_t hardware_priority) {

//    check_irq_param(num);

    // note that only 32 bit writes are supported
    io_rw_32 *p = (io_rw_32 *)((PPB_BASE + M0PLUS_NVIC_IPR0_OFFSET) + (num & ~3u
));
    *p = (*p & ~(0xffu << (8 * (num & 3u)))) | (((uint32_t) hardware_priority) << (8 * (num & 3u)));
}



void irq_init_priorities() {
#if PICO_DEFAULT_IRQ_PRIORITY != 0
    for (uint irq = 0; irq < NUM_IRQS; irq++) {
        irq_set_priority(irq, PICO_DEFAULT_IRQ_PRIORITY);
    }
#endif
}


void *memcpy(void *dest, const void *src, uint32_t n) {

        int i;

        char *d=dest;
        const char *s=src;

        for(i=0;i<n;i++) {
                *d=*s;
                d++;
                s++;
        }

        return dest;
}


void runtime_init(void) {
    // Reset all peripherals to put system into a known state,
    // - except for QSPI pads and the XIP IO bank, as this is fatal if running from flash
    // - and the PLLs, as this is fatal if clock muxing has not been reset on this boot
    reset_block(~(
            RESETS_RESET_IO_QSPI_BITS |
            RESETS_RESET_PADS_QSPI_BITS |
            RESETS_RESET_PLL_USB_BITS |
            RESETS_RESET_PLL_SYS_BITS
    ));

    // Remove reset from peripherals which are clocked only by clk_sys and
    // clk_ref. Other peripherals stay in reset until we've configured clocks.
    unreset_block_wait(RESETS_RESET_BITS & ~(
            RESETS_RESET_ADC_BITS |
            RESETS_RESET_RTC_BITS |
            RESETS_RESET_SPI0_BITS |
            RESETS_RESET_SPI1_BITS |
            RESETS_RESET_UART0_BITS |
            RESETS_RESET_UART1_BITS |
            RESETS_RESET_USBCTRL_BITS
    ));

    // pre-init runs really early since we need it even for memcpy and divide!
    // (basically anything in aeabi that uses bootrom)

    // Start and end points of the constructor list,
    // defined by the linker script.
    extern void (*__preinit_array_start)(void);
    extern void (*__preinit_array_end)(void);

    // Call each function in the list.
    // We have to take the address of the symbols, as __preinit_array_start *is*
    // the first function pointer, not the address of it.
    for (void (**p)(void) = &__preinit_array_start; p < &__preinit_array_end; ++p) {
        (*p)();
    }

    // After calling preinit we have enough runtime to do the exciting maths
    // in clocks_init
    clocks_init();

    // Peripheral clocks should now all be running
    unreset_block_wait(RESETS_RESET_BITS);

#if !PICO_IE_26_29_UNCHANGED_ON_RESET
    // after resetting BANK0 we should disable IE on 26-29
    hw_clear_alias(padsbank0_hw)->io[26] = hw_clear_alias(padsbank0_hw)->io[27] =
            hw_clear_alias(padsbank0_hw)->io[28] = hw_clear_alias(padsbank0_hw)->io[29] = PADS_BANK0_GPIO0_IE_BITS;
#endif

#if 0

    extern mutex_t __mutex_array_start;
    extern mutex_t __mutex_array_end;

    // the first function pointer, not the address of it.
    for (mutex_t *m = &__mutex_array_start; m < &__mutex_array_end; m++) {
        mutex_init(m);
    }

#endif

#if !(PICO_NO_RAM_VECTOR_TABLE || PICO_NO_FLASH)
    __builtin_memcpy(ram_vector_table, (uint32_t *) scb_hw->vtor, sizeof(ram_vector_table));
    scb_hw->vtor = (uintptr_t) ram_vector_table;
#endif

#ifndef NDEBUG
//    if (__get_current_exception()) {
        // crap; started in exception handler
//        __asm ("bkpt #0");
//    }
#endif

#if PICO_USE_STACK_GUARDS
    // install core0 stack guard
    extern char __StackBottom;
    runtime_install_stack_guard(&__StackBottom);
#endif

//    spin_locks_reset();
    irq_init_priorities();

	// disabled by default?
  //  alarm_pool_init_default();

    // Start and end points of the constructor list,
    // defined by the linker script.
    extern void (*__init_array_start)(void);
    extern void (*__init_array_end)(void);

    // Call each function in the list.
    // We have to take the address of the symbols, as __init_array_start *is*
    // the first function pointer, not the address of it.
    for (void (**p)(void) = &__init_array_start; p < &__init_array_end; ++p) {
        (*p)();
    }

}

void _exit(__unused int status) {
#if PICO_ENTER_USB_BOOT_ON_EXIT
    reset_usb_boot(0,0);
#else
    while (1) {
//        __breakpoint();
    }
#endif
}

void *_sbrk(int incr) {
    extern char end; /* Set by linker.  */
    static char *heap_end;
    char *prev_heap_end;

    if (heap_end == 0)
        heap_end = &end;

    prev_heap_end = heap_end;
    char *next_heap_end = heap_end + incr;

    if (__builtin_expect(next_heap_end > (&__StackLimit), 0)) {
#if PICO_USE_OPTIMISTIC_SBRK
        if (heap_end == &__StackLimit) {
//        errno = ENOMEM;
            return (char *) -1;
        }
        next_heap_end = &__StackLimit;
#else
        return (char *) -1;
#endif
    }

    heap_end = next_heap_end;
    return (void *) prev_heap_end;
}

// exit is not useful... no desire to pull in __call_exitprocs
void exit(int status) {
    _exit(status);
}

// incorrect warning from GCC 6
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsuggest-attribute=format"
void __assert_func(const char *file, int line, const char *func, const char *failedexpr) {
//    weak_raw_printf("assertion \"%s\" failed: file \"%s\", line %d%s%s\n",
//           failedexpr, file, line, func ? ", function: " : "",
//           func ? func : "");

    _exit(1);
}

#pragma GCC diagnostic pop

#if 0
void __attribute__((noreturn)) panic_unsupported() {
    panic("not supported");
}
#endif

// todo consider making this try harder to output if we panic early
//  right now, print mutex may be uninitialised (in which case it deadlocks - although after printing "PANIC")
//  more importantly there may be no stdout/UART initialized yet
// todo we may want to think about where we print panic messages to; writing to USB appears to work
//  though it doesn't seem like we can expect it to... fine for now
//
#if 0
void __attribute__((noreturn)) __printflike(1, 0) panic(const char *fmt, ...) {
    puts("\n*** PANIC ***\n");
    if (fmt) {
#if PICO_PRINTF_NONE
        puts(fmt);
#else
        va_list args;
        va_start(args, fmt);
#if PICO_PRINTF_ALWAYS_INCLUDED
        vprintf(fmt, args);
#else
        weak_raw_vprintf(fmt, args);
#endif
        va_end(args);
        puts("\n");
#endif
    }

    _exit(1);
}
#endif

void hard_assertion_failure(void) {
//    panic("Hard assert");
}