/* Write something to the LCD Display */

#include <stdint.h>

#include "stm32l.h"
#include "lcd_lib.h"
#include "delay_lib.h"

unsigned int length=5*148*12+13;	/* us */


int num_to_string(char *string,unsigned int value) {

	int len=0,q,r,nonzero=0;

	if (value==0) {
		string[len]='0';
		len++;
		goto done_num_to_string;
	}

	q=value/1000000;
	r=value%1000000;
	/* too big */
	if (q>10) return -1;

	if (q!=0) {
		string[len]=q+'0';
		len++;
		nonzero=1;
	}

	q=r;
	r=q%100000;
	q/=100000;
	if (nonzero || q!=0) {
		string[len]=q+'0';
		len++;
		nonzero=1;
	}


	q=r;
	r=q%10000;
	q/=10000;
	if (nonzero || q!=0) {
		string[len]=q+'0';
		len++;
		nonzero=1;
	}


	q=r;
	r=q%1000;
	q/=1000;
	if (nonzero || q!=0) {
		string[len]=q+'0';
		len++;
		nonzero=1;
	}


	q=r;
	r=q%100;
	q/=100;
	if (nonzero || q!=0) {
		string[len]=q+'0';
		len++;
		nonzero=1;
	}


	q=r;
	r=q%10;
	q/=10;
	if (nonzero || q!=0) {
		string[len]=q+'0';
		len++;
		nonzero=1;
	}


	q=r;
//	r=q%1;
	q/=1;
	if (nonzero || q!=0) {
		string[len]=q+'0';
		len++;
		nonzero=1;
	}




done_num_to_string:

	string[len+1]=0;

	return len;
}

int main(void) {

	unsigned int lcd_buffer[16];
	unsigned int value;
	char string[7];

	lcd_clock_init();

	lcd_pin_init();

	lcd_config();

	/* busy wait forever */
	for(;;) {

	length+=1;

	value=length/(148*12);

	num_to_string(string,value);
	lcd_convert(string,lcd_buffer);
	lcd_display(lcd_buffer);
	busy_delay(500000);

	lcd_convert("VINCE ",lcd_buffer);
	lcd_display(lcd_buffer);
	busy_delay(500000);

	lcd_convert("WEAVER",lcd_buffer);
	lcd_display(lcd_buffer);
	busy_delay(500000);

	lcd_convert("ABCDEF",lcd_buffer);
	lcd_display(lcd_buffer);
	busy_delay(500000);

	lcd_convert("GHIJKL",lcd_buffer);
	lcd_display(lcd_buffer);
	busy_delay(500000);

	lcd_convert("MNOPQR",lcd_buffer);
	lcd_display(lcd_buffer);
	busy_delay(500000);

	lcd_convert("STUVWX",lcd_buffer);
	lcd_display(lcd_buffer);
	busy_delay(500000);

	lcd_convert("YZ0123",lcd_buffer);
	lcd_display(lcd_buffer);
	busy_delay(500000);

	lcd_convert("456789",lcd_buffer);
	lcd_display(lcd_buffer);
	busy_delay(500000);

	}

}

static void nmi_handler(void) {
	for(;;);
}

static void hardfault_handler(void) {
	for(;;);
}

extern unsigned long _etext,_data,_edata,_bss_start,_bss_end;

	/* Copy DATA and BSS segments into RAM */
void c_startup(void)	{

	unsigned long *src, *dst;

	/* Copy data segment */
	/* Using linker symbols */
	src = &_etext;
	dst = &_data;
	while(dst < &_edata) *(dst++) = *(src++);

	/* Zero out the BSS */
	src = &_bss_start;
	while(src < &_bss_end) *(src++) = 0;

	/* Call main() */
	main();

}

#define STACK_TOP 0x20000800

/* Vector Table */
unsigned int *myvectors[4]
__attribute__ ((section("vectors"))) = {
	(uint32_t *) STACK_TOP,		/* stack pointer      */
	(uint32_t *) c_startup,		/* code entry point   */
	(uint32_t *) nmi_handler,	/* NMI handler        */
	(uint32_t *) hardfault_handler	/* hard fault handler */
};
