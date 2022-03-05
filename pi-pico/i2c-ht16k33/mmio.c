#include <stdint.h>

#include "include/mmio.h"


#define REG_ALIAS_SET_BITS (0x2u << 12u)
#define REG_ALIAS_CLR_BITS (0x3u << 12u)

#define hw_set_alias_untyped(addr) ((void *)(REG_ALIAS_SET_BITS | (uintptr_t)(addr)))
#define hw_clear_alias_untyped(addr) ((void *)(REG_ALIAS_CLR_BITS | (uintptr_t)(addr)))
#define hw_clear_alias(p) ((typeof(p))hw_clear_alias_untyped(p))

void hw_set_bits(io_rw_32 *addr, uint32_t mask) {
    *(io_rw_32 *) hw_set_alias_untyped((volatile void *) addr) = mask;
}

void hw_clear_bits(io_rw_32 *addr, uint32_t mask) {
    *(io_rw_32 *) hw_clear_alias_untyped((volatile void *) addr) = mask;
}


#define REG_ALIAS_XOR_BITS (0x1u << 12u)

#define hw_xor_alias_untyped(addr) ((void *)(REG_ALIAS_XOR_BITS | (uintptr_t)(addr)))

/* Atomically flip the specified bits in a HW register */
static void hw_xor_bits(io_rw_32 *addr, uint32_t mask) {
    *(io_rw_32 *) hw_xor_alias_untyped((volatile void *) addr) = mask;
}


void hw_write_masked(io_rw_32 *addr,
		uint32_t values, uint32_t write_mask) {
	hw_xor_bits(addr, (*addr ^ values) & write_mask);
}

void tight_loop_contents(void) {

}
