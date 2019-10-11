#include <stdint.h>

static inline uint32_t get_CPSR(void) {

	uint32_t temp;

	asm volatile ("mrs %0,CPSR":"=r" (temp):) ;

	return temp;
}

static inline void set_CPSR(uint32_t new_cpsr) {
	asm volatile ("msr CPSR_cxsf,%0"::"r"(new_cpsr) );
}



/* enable interrupts  */
static inline void enable_interrupts(void){
	uint32_t temp;
	temp = get_CPSR();
	set_CPSR(temp & ~0x80);
}

/* Disable interrupts */
static inline uint32_t disable_interrupts(void){
	uint32_t temp;
	temp = get_CPSR();
	set_CPSR(temp | 0x80);
	return temp;
}


