// from src/rp2_common/hardware_base/include/hardware/address_mapped.h
typedef volatile uint32_t io_rw_32;
typedef const volatile uint32_t io_ro_32;
typedef volatile uint32_t io_wo_32;

void hw_set_bits(io_rw_32 *addr, uint32_t mask);
void hw_clear_bits(io_rw_32 *addr, uint32_t mask);
void hw_write_masked(io_rw_32 *addr, uint32_t values, uint32_t write_mask);
void tight_loop_contents(void);
