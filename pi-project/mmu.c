/* From discussion here:
 http://stackoverflow.com/questions/3439708/how-to-enable-arm1136jfs-arm-v6-mmu-to-have-one-to-one-mapping-between-physica
*/

#include <stdint.h>

#include "printk.h"

/* From the ARM1176JZF-S Technical Reference Manual	*/
/* The processor has separate L1I and L1D caches	*/
/* Separate I and D tightly-coupled memory (TCM)	*/
/* Two micro-TLBs backed by a main TLB			*/

/* Caches are VIPT */
/* Linesize is 32 bytes (8 words) */
/* On BCM2835 cache sizes are 16kb and 4-way */

/* By default caches have pseudo-random replacement */
/* This can be configured to round-robin in the control register (bit 14) */



/* We want to cover all 4GB of address space	*/
/* Using 1MB pages, so need 4096 entries	*/
#define NUM_PAGE_TABLE_ENTRIES 4096

/* make sure properly aligned, as the low bits are reserved  */
/* we're assuming here that 14 bits are reserved? */
uint32_t  __attribute__((aligned(16384))) page_table[NUM_PAGE_TABLE_ENTRIES];

/* We want a 1MB coarse ARMv5 page table descriptor */
/* see 6.11.1 and 6.12.2 and 6.6.1 */
/* All mappings global an executable */
/* 31-20 = section base address
   19 = NS (not shared)
   18 = 0
   17-15 = SBZ (should be zero)
   14-12 = TEX = Type extension field
   11-10 - AP (access permission bits)
   9 = P - has to do with alizaing
   8-5 = domain
   4 = 0
   3,2 = C,B determinte caching behavior, see Tables 6.2 and 6.3
   1,0 = 1,0 - for ARMv5 1MB pages
*/

/* Domain=1, C=0,B=0, noncachable (Table 6.3) */
#define CACHE_DISABLED    0x12
/* Domain=1, C=1,B=1, writeback cache, no alloc on write (Table 6.3) */
#define CACHE_WRITEBACK   0x1e


/* Enable a one-to-one physical to virtual mapping using 1MB pagetables */
/* This uses the ARMv5 compatible interface, not native ARMv6 */
/* Mark RAM has writeback, but disable cache for non-RAM */
void enable_mmu(uint32_t mem_start, uint32_t mem_end) {

	int i;
	uint32_t reg;

	/* Set up an identity-mapping for all 4GB, ARMv5 1MB pages */
	/* AP (bits 11 and 10) = 11 = R/W for everyone */
	for (i = 0; i < NUM_PAGE_TABLE_ENTRIES; i++) {
		page_table[i] = i << 20 | (3 << 10) | CACHE_DISABLED;
	}

	/* Then, enable cacheable and bufferable for RAM only */
	for (i = mem_start >> 20; i < mem_end >> 20; i++) {
		page_table[i] = i << 20 | (3 << 10) | CACHE_WRITEBACK;
	}

	/* Copy the page table address to cp15 */
	/* Translation Table, Base 0 */
	/* See 3.2.13 */
	/* Bits 31-N are the address of the table */
	/* Low bits are various config options, we leave them at 0 */
	asm volatile("mcr p15, 0, %0, c2, c0, 0"
		: : "r" (page_table) : "memory");

	/* See 3.2.16 */
	/* Set the access control register */
	/* All domains, set manager access (no faults for accesses) */
	asm volatile("mcr p15, 0, %0, c3, c0, 0" : : "r" (~0));

	/* See 3.2.7 */
	/* Set the Control Register */
	/* Enable the MMU by setting the M bit */
	asm("mrc p15, 0, %0, c1, c0, 0" : "=r" (reg) : : "cc");
	reg|=0x1;
	asm volatile("mcr p15, 0, %0, c1, c0, 0" : : "r" (reg) : "cc");
}

/* See 1176 manual, 3.2.7 */

/* Also note that you *must* enable the MMU before using the dcache	*/
/* Why?  hard to find in ARM manuals.  They do have a FAQ for ARM9	*/
/* Why must I enable the MMU to use the D-Cache but not for the I-Cache?*/
/* TLDR: by default the dcache would cache everything, including MMIO	*/
/*       accesses, so you need the MMU enabled so you can mark the MMIO */
/*       regions as non-cachable					*/

void enable_l1_dcache(void) {
	/* load control register to r0 */
	asm volatile( "mrc p15, 0, r0, c1, c0, 0" );
	/* set bit 12: enable dcache */
	asm volatile( "orr r0, r0, #4" );
	/* store back out to control register */
	asm volatile( "mcr p15, 0, r0, c1, c0, 0" );
}

/* See 3.2.22 */
void disable_l1_dcache(void) {
	/* FIXME */
	/* Need to clear out and invalidate all entries in cache first */
	/* Also may need to disable L1 icache and branch-target buffer too */
}

/* See 1176 manual, 3.2.7 */
void enable_l1_icache(void) {
	/* load control register to r0 */
	asm volatile( "mrc p15, 0, r0, c1, c0, 0" );
	/* set bit 12: enable icache */
	asm volatile( "orr r0, r0, #4096" );
	/* store back out to control register */
	asm volatile( "mcr p15, 0, r0, c1, c0, 0" );
}

void disable_l1_icache(void) {
	/* load control register to r0 */
	asm volatile( "mrc p15, 0, r0, c1, c0, 0" );
	/* clear bit 12: enable icache */
	asm volatile( "bic r0, r0, #4096" );
	/* store back out to control register */
	asm volatile( "mcr p15, 0, r0, c1, c0, 0" );
}

/* See 1176 manual, 3.2.7 */
/* Z-bit */
/* Also see 3.2.8 Auxiliary Control Register */
/* Bit 2 (SB) = static branch prediction (on by default) */
/* Bit 1 (DB) = dynamic branch prediction (on by default) */
/* Bit 0 (RS) = return stack prediction (on by default) */
void enable_branch_predictor(void) {
	/* load control register to r0 */
	asm volatile( "mrc p15, 0, r0, c1, c0, 0" );
	/* set bit 11: enable branch predictor */
	asm volatile( "orr r0, r0, #2048" );
	/* store back out to control register */
	asm volatile( "mcr p15, 0, r0, c1, c0, 0" );
}

void disable_branch_predictor(void) {
	/* load control register to r0 */
	asm volatile( "mrc p15, 0, r0, c1, c0, 0" );
	/* clear bit 12: enable icache */
	asm volatile( "bic r0, r0, #2048" );
	/* store back out to control register */
	asm volatile( "mcr p15, 0, r0, c1, c0, 0" );
}



static uint32_t convert_size(uint32_t value) {

	switch(value) {
		case 0:	return 0; /* 0.5kB, not supported */
		case 1:	return 1; /* not supported */
		case 2:	return 2; /* not supported */
		case 3: return 4;
		case 4: return 8;
		case 5:	return 16;
		case 6: return 32;
		case 7:	return 64;
		case 8: return 128; /* not supported */
		default: return 0;
	}
}

/* see 3.2.3 in the 1176 document */
/* cp15/c0/1 */
void l1_cache_detect(void) {

	uint32_t reg;
	uint32_t size,assoc,blocksize,restrict;

	asm("mrc p15, 0, %0, c0, c0, 1" : "=r" (reg) : : "cc");

	/* 28-25 = type, 1110 on 1176 */
	/* 24 = separate I and D caches */

	restrict=!!(reg&(1<<24));
	size=convert_size((reg>>18)&0xf);
	assoc=(reg>>15)&0x7;
	blocksize=(reg>>12)&3;

	printk("Detected L1 data cache: "
		"%d size, %d assoc, %d blocksize, %d restrict\n",
		size,assoc,blocksize,restrict);

	restrict=!!(reg&(1<<11));
	size=convert_size((reg>>6)&0xf);
	assoc=(reg>>3)&0x7;
	blocksize=(reg>>0)&3;

	printk("Detected L1 instruction cache: "
		"%d size, %d assoc, %d blocksize, %d restrict\n",
		size,assoc,blocksize,restrict);
}


