




/* A simple clock appication.  */
/* Prints the time on the alphanum segments */
/* Puts UNIX time along the bargraphs segments */
/* Day of the week on the colored LEDs */
/* Occasionally the month/day/year is also displayed */


// Day/date on 8x16
// Unix time on green bargraphs
// Time/ AM/PM on Alphanum

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <ctype.h>
#include <signal.h>

#include "ay-3-8910.h"
#include "max98306.h"
#include "display.h"

static int display_type=DISPLAY_I2C;
static int play_music=0;


// ***  *   *  **  * * ***  ** *** *** ***
// * * **  * *   * * * *   *     * * * * *
// * *  *    *  ** *** **  ***  *  *** ***
// * *  *   *    *   *   * * *  *  * *   *
// *** *** *** **    * **  ***  *  *** **

static unsigned char three_by_five[128][3]={
/* 0 */	{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},
/* 8 */	{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},
/* 16*/	{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},
/* 24*/	{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},
/* 32*/	{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},
/* 40*/	{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},
/*'0'*/	{0xf8,0x88,0xf8},{0x48,0xf8,0x08},{0x48,0x98,0x68},{0x88,0xa8,0x70},
/*'4'*/ {0xe0,0x20,0xf8},{0xe8,0xa8,0x90},{0x78,0xa8,0xb8},{0x80,0xb8,0xc0},
/*'8'*/ {0xf8,0xa8,0xf8},{0xe8,0xa8,0xf0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},
/*'@'*/ {0,0,0},         {0x78,0xa0,0x78},{0,0,0},{0,0,0},
/*'D'*/ {0,0,0},         {0xf8,0xa8,0x88},{0xf8,0xa0,0x80},{0,0,0},
/*'H'*/ {0xf8,0x20,0xf8},{0x0,0x0,0x0},   {0x0,0x0,0x0}, {0,0,0},
/*'L'*/ {0,0,0},	 {0xf8,0x40,0xf8},{0x0,0x0,0x0}, {0xf8,0x88,0xf8},
/*'P'*/ {0,0,0},	 {0x0,0x0,0x0},   {0xf8,0xb0,0xe8}, {0x48,0xa8,0xb0},
/*'T'*/ {0x80,0xf8,0x80},{0xf8,0x08,0xf8}, {0,0,0}, {0xf8,0x10,0xf8},
/*'X'*/ {0x0,0x0,0x0},   {0x0,0x0,0x0}, {0,0,0},{0x0,0x0,0x0},
};

//  ** * *  * * ***  *** * *  * * ***  *** * *  *** ***   **  *
// *   * *  *** * *   *  * *  * * *     *  * *  *   * *  *   * *
//  ** * *  * * * *   *  * *  * * **    *  ***  **  ***   ** ***
//   * * *  * * * *   *  * *  *** *     *  * *  *   **     * * *
// **  ***  * * ***   *  ***  * * ***   *  * *  *   * *  **  * *


static char day_abbreviations[7][3]={
	"SU","MO","TU","WE","TH","FR","SA"
};


static int alarm_hour=10;
static int alarm_minute=00;


static int reverse_bits32(int v) {

	unsigned int s = 32;
	unsigned int mask = ~0;
	while ((s >>= 1) > 0) {
		mask ^= (mask << s);
		v = ((v >> s) & mask) | ((v << s) & ~mask);
	}

	return v;

}

static void quiet_and_exit(int sig) {

	if (play_music) {
		quiet_ay_3_8910(16);
		close_ay_3_8910();
		max98306_free();
	}

	display_shutdown(display_type);

	printf("Quieting and exiting\n");
	_exit(0);

}



int main(int argc, char **argv) {

	int i;

	time_t seconds;  /* Y2038 problem! */
	unsigned int reverse_seconds;
	int result;

	struct tm *breakdown;
	char out_string[13];

	int last_minute=-1;
	unsigned char big_display[16];
	int digit;

#if 0
    struct tm {
      int tm_sec;         /* seconds */
      int tm_min;         /* minutes */
      int tm_hour;        /* hours */
      int tm_mday;        /* day of the month */
      int tm_mon;         /* month */
      int tm_year;        /* year */
      int tm_wday;        /* day of the week */
      int tm_yday;        /* day in the year */
      int tm_isdst;       /* daylight saving time */
    };
#endif

	/* Setup control-C handler to quiet the music   */
	/* otherwise if you force quit it keeps playing */
	/* the last tones */
	signal(SIGINT, quiet_and_exit);

	result=display_init(DISPLAY_I2C);
	if (result<0) {
		display_type=DISPLAY_TEXT;
	}

	while(1) {
		seconds=time(NULL);

		breakdown=localtime(&seconds);

		reverse_seconds=reverse_bits32(seconds);

		/* binary clock around eddge */
		bargraph_raw(display_type,
			(seconds>>24)&0xff,		// la = 24
			(seconds>>8)&0xff,		// lb = 8
			(1<<((breakdown->tm_sec/6)+1))-1, // lc
			(reverse_seconds>>8)&0xff,	// ra = 16
			(reverse_seconds>>24)&0xff,	// rb = 0
			(1<<((breakdown->tm_sec/6)+1))-1); // rc

//		display_state[7]=(reverse_seconds>>16)&0xffff;
//		display_state[6]=(seconds>>16)&0xffff;



		if ((alarm_hour==breakdown->tm_hour) &&
			(alarm_minute==breakdown->tm_min)) {
			if (seconds&1) sprintf(out_string,"* WAKE UP! *");
			else sprintf(out_string,"            ");
		}

		else {
		if (seconds&1) {
			sprintf(out_string,"  %2d:%02d %s  ",
				breakdown->tm_hour%12,
				breakdown->tm_min,
				breakdown->tm_hour>11?"PM":"AM");
		}
		else {
			sprintf(out_string,"  %2d %02d %s  \n",
				breakdown->tm_hour%12,
				breakdown->tm_min,
				breakdown->tm_hour>11?"PM":"AM");

		}
		}
		display_14seg_string(display_type,out_string);

		if (breakdown->tm_min!=last_minute) {
			last_minute=breakdown->tm_min;

			for(i=0;i<16;i++) big_display[i]=0;

			digit=day_abbreviations[breakdown->tm_wday][0];
			big_display[0]=three_by_five[digit][0];
			big_display[1]=three_by_five[digit][1];
			big_display[2]=three_by_five[digit][2];

			digit=day_abbreviations[breakdown->tm_wday][1];
			big_display[4]=three_by_five[digit][0];
			big_display[5]=three_by_five[digit][1];
			big_display[6]=three_by_five[digit][2];

			digit=(breakdown->tm_mday/10)+'0';
			if (digit!='0') {
				big_display[ 9]=three_by_five[digit][0];
				big_display[10]=three_by_five[digit][1];
				big_display[11]=three_by_five[digit][2];
			}

			digit=(breakdown->tm_mday%10)+'0';
			big_display[13]=three_by_five[digit][0];
			big_display[14]=three_by_five[digit][1];
			big_display[15]=three_by_five[digit][2];

			display_8x16_vertical(display_type,big_display);
		}

		usleep(200000);
	}

	display_shutdown(display_type);

	return 0;
}


