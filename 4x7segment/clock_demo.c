/* Makes a clock */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include "i2c_lib.h"

unsigned char digits[16][8] = {
	{0,1,1,1,1,1,1,0},	// 0
	{0,0,0,0,1,1,0,0},	// 1
	{1,0,1,1,0,1,1,0},	// 2
	{1,0,0,1,1,1,1,0},	// 3
	{1,1,0,0,1,1,0,0},	// 4
	{1,1,0,1,1,0,1,0},	// 5
	{1,1,1,1,1,0,1,0},	// 6
	{0,0,0,0,1,1,1,0},	// 7
	{1,1,1,1,1,1,1,0},	// 8
	{1,1,0,1,1,1,1,0},	// 9
	{1,1,1,0,1,1,1,0},	// A
	{1,1,1,1,1,0,0,0},	// B
	{0,1,1,1,0,0,1,0},	// C
	{1,0,1,1,1,1,0,0},	// D
	{1,1,1,1,0,0,1,0},	// E
	{1,1,1,0,0,0,1,0},	// F
};


int main(int argc, char **argv) {

	unsigned char display_buffer[8];
	int result,i;
	int blink=0;

	time_t seconds;
	struct tm *breakdown;

	result=init_display("/dev/i2c-3",10);

	if (result) {
		fprintf(stderr,"Error opening device!\n");
		return -1;
	}


/*

The adafruit setup for these displays is crazy.
Not sure why they couldn't make it much more
straightforward.

   --6A-
  |     |
  1F    5B       : = 5
  |     |
   -G0--
  |     |
  2E    4C
  |     |
   -3D--        . = 7DP


  number = which display_buffer[]

  8  8  :  8  8
 128 64 32 16 8

 so to set segment 0 of the far right element, then
     display_buffer[0]=128;

 To put 0 in the far right display

	display_buffer[1]=8;
	display_buffer[2]=8;
	display_buffer[3]=8;
	display_buffer[4]=8;
	display_buffer[5]=8;
	display_buffer[6]=8;


*/
	while(1) {

		/* clear display */
		for(i=0;i<8;i++) display_buffer[i]=0x00;

		seconds=time(NULL);
		breakdown=localtime(&seconds);

		for(i=0;i<8;i++) {

			/* hour */
			if (digits[(breakdown->tm_hour / 10)][i]) {
                           display_buffer[i]|=0x80;
			}
			if (digits[(breakdown->tm_hour % 10)][i]) {
                           display_buffer[i]|=0x40;
			}


			/* minutes */
			if (digits[(breakdown->tm_min / 10)][i]) {
                           display_buffer[i]|=0x10;
			}
			if (digits[(breakdown->tm_min % 10)][i]) {
                           display_buffer[i]|=0x08;
			}
		}
		blink=!blink;
		if (blink) {
			display_buffer[5]|=0x20;
		}


		update_display(display_buffer);

		usleep(500000);
	}

	return 0;

}

