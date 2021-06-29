/*
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


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

static void unreset_block_wait(uint32_t bits) {
    hw_clear_bits(&resets_hw->reset, bits);
    while (~resets_hw->reset_done & bits)
        tight_loop_contents();
}

