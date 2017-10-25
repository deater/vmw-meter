/* This code assumes you have a VMW-HARDWARE-CHIPTUNES-DISPLAY-MK1        */
/* This includes five ht16k33 based displays hooked to the first i2c bus: */
/* + A ht16k33 breakout with six 10-segment GYR bargraphs (0x70)	  */
/*   also 8 buttons are hooked up to the ht16k33 breakout.		  */
/* + An adafruit 8x16 led matrix backpack (0x72) 			  */
/* + Three adafruit 14seg led backpacks (..,..,..)			  */

/* Be sure to modprobe i2c-dev */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <termios.h>
#include <fcntl.h>

#include "stats.h"
#include "display.h"

#include "visualizations.h"

int display_update(int display_type,
		struct display_stats *ds,
		int current_frame, int num_frames,
		char *filename, int new_filename,
		int current_mode) {


	bargraph_filled(display_type,ds);

	switch(current_mode) {
		case MODE_TITLE:
			display_8x16_title(display_type);
			break;
		case MODE_VISUAL:
			display_8x16_freq(display_type,ds);
			break;
		case MODE_NAME:
			display_8x16_scroll_text(display_type, filename, new_filename);
			break;
		case MODE_TIME:
			display_8x16_time(display_type, current_frame, num_frames);
			break;
		case MODE_NOTEMATRIX:
			display_8x16_notematrix(display_type,ds);
			break;
		case MODE_PIANO:
			display_8x16_piano(display_type,ds);
			break;
		default:
			printf("Unknown visual mode!\n");
			break;
	}

	return 0;
}
