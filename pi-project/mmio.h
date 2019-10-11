static inline void mmio_write(uint32_t address, uint32_t data) {
	uint32_t *ptr = (uint32_t *)address;
	asm volatile("str %[data], [%[address]]" :
		: [address]"r"(ptr), [data]"r"(data));
}

static inline uint32_t mmio_read(uint32_t address) {
	uint32_t *ptr = (uint32_t *)address;
	uint32_t data;
	asm volatile("ldr %[data], [%[address]]" :
		[data]"=r"(data) : [address]"r"(ptr));
	return data;
}
