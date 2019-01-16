#include "delay_lib.h"

/* 500k is roughly 3s on a stm32l */
void busy_delay(int length) {

	volatile int i;

	for(i=0;i<length;i++) asm("nop");

}
