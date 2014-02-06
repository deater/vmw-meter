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
	char string[7],number[7];
	unsigned int count=0,feet,inches,length;

	lcd_clock_init();

	lcd_pin_init();

	lcd_config();
loop:
	/* busy wait forever */
	for(count=0;count<2048;count++) {

		feet=count/12;
		inches=count%12;

		length=num_to_string(number,feet);

		if (length==1) {
			string[0]=' ';
			string[1]=number[0];
		}
		else {
			string[0]=number[0];
			string[1]=number[1];
		}
		string[2]='\'';


		length=num_to_string(number,inches);

		if (length==1) {
			string[3]=' ';
			string[4]=number[0];
		}
		else {
			string[3]=number[0];
			string[4]=number[1];
		}
		string[5]='\"';


		lcd_convert(string,lcd_buffer);
		lcd_display(lcd_buffer);
		busy_delay(100000);

	}

	goto loop;

	while(1);

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
