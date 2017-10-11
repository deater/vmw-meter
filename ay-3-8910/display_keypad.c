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

#if USE_LINUX_I2C==1
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include "i2c_lib.h"
#else
#include <bcm2835.h>
#endif





/*        4 5        */
/*        3 6        */
/* 2               7 */
/* 1               8 */

/*       Play Stop   */
/*       Mode X      */
/*  <              > */
/* <<             >> */

/*       ' ' s       */
/*       m   x       */
/*  <              > */
/*  ,              . */


int display_raw_keypad_read(int display_type) {

	unsigned char ch=0;
	long long keypad;
	static int old_keypad=0;
	static int keypad_skip=0;

	/* Read from keypad */
	if ((display_type&DISPLAY_I2C) && (keypad_skip==0)) {
		keypad=read_keypad(i2c_fd,HT16K33_ADDRESS0);
		if (keypad!=old_keypad) {
			//printf("KEY: %lld\n",keypad);
			old_keypad=keypad;
			if (keypad!=0) {
				if (keypad&32) ch=CMD_RW;	/* rewind */
				if (keypad&64) ch=CMD_BACK;	/* back */
				if (keypad&128) ch=CMD_MENU;	/* menu */
				if (keypad&256) ch=CMD_PLAY;	/* play */
				if (keypad&512) ch=CMD_STOP;	/* stop */
				if (keypad&1024) ch=CMD_CANCEL;	/* cancel */
				if (keypad&2048) ch=CMD_NEXT;	/* next */
				if (keypad&4096) ch=CMD_FF;	/* ffwd */
			}
		}
	}
	keypad_skip++;
	if (keypad_skip>2) keypad_skip=0;

	return ch;

}

int display_keypad_read(int display_type) {

	unsigned char ch;
	int result;
	int keypad_result=0;

	/* read from keypad first */
	keypad_result=display_raw_keypad_read(display_type);
	if (keypad_result) {
		return keypad_result;
	}

	/* Read from Keyboard, emulate keypad */
	result=read(0,&ch,1);

	if (result<0) { //printf("Error %s\n",strerror(errno));
	}
	else {
		switch(ch) {

			/* Number Row */
			case '-': /* volume down */
				return CMD_VOL_DOWN;
				break;
			case '=':
			case '+':
				return CMD_VOL_UP;
				break;
			case '1':
				return CMD_HEADPHONE_IN;
				break;
			case '2':
				return CMD_HEADPHONE_OUT;
				break;

			/* Top Row */
			case 'q':
			case 'Q':
			case 27:
				/* quit */
				return CMD_EXIT_PROGRAM;
				break;

			/* e through p reserved for piano keyboard */


			/* Middle Row */
			case 'a':
				return CMD_MUTE_A;
				break;
			case 's':
				return CMD_STOP;
				break;
			case 'l': /* toggle loop */
				return CMD_LOOP;
				break;

			/* Bottom Row */
			case 'x':
				return CMD_CANCEL;
				break;
			case 'c':
				return CMD_MUTE_C;
				break;
			case 'b':
				return CMD_MUTE_B;
				break;
			case 'n':
				return CMD_MUTE_NA;
				break;
			case 'o':
				return CMD_MUTE_NB;
				break;
			case 'p':
				return CMD_MUTE_NC;
				break;

			case 'm': /* mode */
				return CMD_MENU;
				break;
			case ',': /* rewind */
				return CMD_RW;
				break;
			case '<': /* back */
				return CMD_BACK;
				break;
			case '.': /* fast-forward */
				return CMD_FF;
				break;
			case '>': /* next */
				return CMD_NEXT;
				break;

			/* Space Row */
			case ' ': /* pause/play */
				return CMD_PLAY;
				break;

		}
	}

	return 0;
}



int display_keypad_clear(int display_type) {

	int ch;

	while(1) {

		ch=display_keypad_read(display_type);
		if (ch==0) break;
//		printf("Read %d\n",ch);
	}

	return 0;
}

int display_keypad_repeat_until_keypressed(int display_type) {

	int ch;

	while(1) {
		ch=display_keypad_read(display_type);
		if (ch) break;
		usleep(10000);
	}

	return ch;
}
