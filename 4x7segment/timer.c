/* Makes a clock */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include "i2c_lib.h"

unsigned short digits[16] = {
	0x003f,	// 0
	0x0006,	// 1
	0x005b,	// 2
	0x004f,	// 3
	0x0066,	// 4
	0x006d, // 5
	0x007d, // 6
	0x0007, // 7
	0x007f, // 8
	0x0067, // 9
	0x0077, // A
	0x007c, // B
	0x0039, // C
	0x005e, // D
	0x0079, // E
	0x0071, // F
};


int main(int argc, char **argv) {

	unsigned short display_buffer[8];
	int i;
	int blink=0;
	int i2c_fd;

	time_t start_time,current_time,elapsed;
	int minutes,seconds,old_seconds=0;

	/* Init i2c */
	i2c_fd=init_i2c("/dev/i2c-1");
	if (i2c_fd < 0) {
		fprintf(stderr,"Error opening device!\n");
		return -1;
	}

	/* Init display */
        if (init_display(i2c_fd,HT16K33_ADDRESS0,10)) {
		fprintf(stderr,"Error opening display\n");
		return -1;
        }


/*
   --0A-
  |     |
  5F    1B       : = 1
  |     |
   -G6--
  |     |
  4E    2C
  |     |
   -3D--        . = 7DP


  8  8  :  8  8
  0  1  2  3  4  Column

 so to set segment G of the far left element, then 16 bit display_buffer

    display_buffer[0]=1<<6; // 0x40

 To put 0 in the far right display

	display_buffer[4]=0x003f;

*/
	start_time=time(NULL);

	while(1) {

		/* clear display */
		for(i=0;i<8;i++) display_buffer[i]=0x00;

		current_time=time(NULL);
		elapsed=current_time-start_time;

		minutes=elapsed/60;
		seconds=elapsed%60;

		/* minutes */
		display_buffer[0]=digits[(minutes/ 10)];
		display_buffer[1]=digits[(minutes% 10)];

		/* seconds */
		display_buffer[3]=digits[(seconds / 10)];
		display_buffer[4]=digits[(seconds % 10)];

		if (seconds!=old_seconds) blink=0;
		old_seconds=seconds;

		blink++;
		if (blink>5) {
			display_buffer[2]|=0x02;
		}
		if (blink>10) {
			blink=0;
		}

		update_display(i2c_fd,HT16K33_ADDRESS0,display_buffer);

		usleep(100000);
	}

	return 0;

}

