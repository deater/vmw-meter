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

#include "ymlib/stats.h"

#include "display.h"

#if USE_LINUX_I2C==1
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include "i2c_lib.h"
#else
#include <bcm2835.h>
#endif

//static int current_mode=MODE_VISUAL;
//static int kiosk_mode=0;



#if USE_LINUX_I2C==1
int i2c_fd=-1;
#else
#define DISPLAY_LINES	8
#endif


static unsigned char display_buffer[DISPLAY_LINES];


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

static struct termios saved_tty;


int display_init(int type) {

	int result=0;
	int i;

	struct termios new_tty;

	/* Save currenty term settings */
	tcgetattr (0, &saved_tty);
	tcgetattr (0, &new_tty);

	/* Put term in raw keryboard mode */
	new_tty.c_lflag &= ~ICANON;
	new_tty.c_cc[VMIN] = 1;
	new_tty.c_lflag &= ~ECHO;
	tcsetattr (0, TCSANOW, &new_tty);
	/* Make it nonblocking too */
	fcntl (0, F_SETFL, fcntl (0, F_GETFL) | O_NONBLOCK);


	if (type&DISPLAY_I2C) {

#if USE_LINUX_I2C==1
		i2c_fd=init_i2c("/dev/i2c-1");
		if (i2c_fd < 0) {
			fprintf(stderr,"Error opening /dev/i2c-1 device!\n");
			return -1;
 		}

		/* Init bargraph/keypad */
		if (init_display(i2c_fd,HT16K33_ADDRESS0,10)) {
			fprintf(stderr,"Error opening bargraph display %x\n",
				HT16K33_ADDRESS0);
			return -1;
		}

		/* ALPHANUM #1 */
		if (init_display(i2c_fd,HT16K33_ADDRESS3,10)) {
			fprintf(stderr,"Error opening 14seg-1 display %x\n",
				HT16K33_ADDRESS3);
			return -1;
		}

		/* ALPHANUM #2 */
		if (init_display(i2c_fd,HT16K33_ADDRESS7,10)) {
			fprintf(stderr,"Error opening 14seg-2 display %x\n",
				HT16K33_ADDRESS7);
			return -1;
		}

		/* ALPHANUM #3 */
		if (init_display(i2c_fd,HT16K33_ADDRESS5,10)) {
			fprintf(stderr,"Error opening 14seg-3 display %x\n",
				HT16K33_ADDRESS5);
			return -1;
		}

		/* Init 8x16 display */
		if (init_display(i2c_fd,HT16K33_ADDRESS2,10)) {
			fprintf(stderr,"Error opening 8x16 display %x\n",
				HT16K33_ADDRESS2);
			return -1;
		}

		/* clear out any lingering keypresses */
		display_keypad_clear(DISPLAY_I2C);

#else

#endif

		for(i=0;i<DISPLAY_LINES;i++) display_buffer[i]=0;


	}

	/* Setup the 14seg font */
	setup_14seg_font();

	return result;
}

int display_shutdown(int display_type) {

	/* read any lingering keypad presses */
	if (display_type&DISPLAY_I2C) {
#if 1
		read_keypad(i2c_fd,HT16K33_ADDRESS0);
#endif
	}

	if (display_type) {
		close_8x16_display(display_type);
		close_bargraph(display_type);
		close_14seg(display_type);
	}

	/* restore keyboard */
	tcsetattr (0, TCSANOW, &saved_tty);

	return 0;
}


