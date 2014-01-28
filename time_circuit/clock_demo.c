/* Makes a clock */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <ctype.h>

#include "i2c_lib.h"

#define SEG_A 0x0001
#define SEG_B 0x0002
#define SEG_C 0x0004
#define SEG_D 0x0008
#define SEG_E 0x0010
#define SEG_F 0x0020
#define SEG_G 0x0040
#define SEG_H 0x0080
#define SEG_K 0x0100
#define SEG_M 0x0200
#define SEG_N 0x0400
#define SEG_P 0x0800
#define SEG_R 0x2000 /* Hooked up R and S backwards? */
#define SEG_S 0x1000 /* hmmmm			     */
#define SEG_T 0x4000
#define SEG_U 0x8000

unsigned short font_7seg[10] = {
	SEG_A|SEG_B|SEG_C|SEG_D|SEG_E|SEG_F,	// 0
	SEG_B|SEG_C,				// 1
	SEG_A|SEG_B|SEG_G|SEG_E|SEG_D,		// 2
	SEG_A|SEG_B|SEG_C|SEG_D|SEG_G,		// 3
	SEG_B|SEG_C|SEG_F|SEG_G,		// 4
	SEG_A|SEG_C|SEG_D|SEG_F|SEG_G,		// 5
	SEG_A|SEG_C|SEG_D|SEG_E|SEG_F|SEG_G,	// 6
	SEG_A|SEG_B|SEG_C,			// 7
	SEG_A|SEG_B|SEG_C|SEG_D|SEG_E|SEG_F|SEG_G,	// 8
	SEG_A|SEG_B|SEG_C|SEG_F|SEG_G,		// 9
};

unsigned short font_16seg[128] = {
	0x0000,	// 0x00
	0x0000,	// 0x01
	0x0000,	// 0x02
	0x0000,	// 0x03
	0x0000,	// 0x04
	0x0000,	// 0x05
	0x0000,	// 0x06
	0x0000,	// 0x07
	0x0000,	// 0x08
	0x0000,	// 0x09
	0x0000,	// 0x0a
	0x0000,	// 0x0b
	0x0000,	// 0x0c
	0x0000,	// 0x0d
	0x0000,	// 0x0e
	0x0000,	// 0x0f
	0x0000,	// 0x10
	0x0000,	// 0x11
	0x0000,	// 0x12
	0x0000,	// 0x13
	0x0000,	// 0x14
	0x0000,	// 0x15
	0x0000,	// 0x16
	0x0000,	// 0x17
	0x0000,	// 0x18
	0x0000,	// 0x19
	0x0000,	// 0x1a
	0x0000,	// 0x1b
	0x0000,	// 0x1c
	0x0000,	// 0x1d
	0x0000,	// 0x1e
	0x0000,	// 0x1f
	0x0000,	// 0x20 ' '
	0x0000,	// 0x21
	0x0000,	// 0x22
	0x0000,	// 0x23
	0x0000,	// 0x24
	0x0000,	// 0x25
	0x0000,	// 0x26
	0x0000,	// 0x27
	0x0000,	// 0x28
	0x0000,	// 0x29
	0x0000,	// 0x2a
	0x0000,	// 0x2b
	0x0000,	// 0x2c
	0x0000,	// 0x2d
	0x0000,	// 0x2e
	0x0000,	// 0x2f
	0x0000,	// 0x30
	0x0000,	// 0x31
	0x0000,	// 0x32
	0x0000,	// 0x33
	0x0000,	// 0x34
	0x0000,	// 0x35
	0x0000,	// 0x36
	0x0000,	// 0x37
	0x0000,	// 0x38
	0x0000,	// 0x39
	0x0000,	// 0x3a
	0x0000,	// 0x3b
	0x0000,	// 0x3c
	0x0000,	// 0x3d
	0x0000,	// 0x3e
	0x0000,	// 0x3f
	0x0000,	// 0x40 '@'
	SEG_A|SEG_B|SEG_C|SEG_D|SEG_G|SEG_H|SEG_U|SEG_P,	// 0x41 'A'
	SEG_A|SEG_B|SEG_E|SEG_F|SEG_G|SEG_H|SEG_N|SEG_R|SEG_U,	// 0x42 'B'
	SEG_A|SEG_B|SEG_E|SEG_F|SEG_G|SEG_H,			// 0x43 'C'
	SEG_A|SEG_B|SEG_C|SEG_D|SEG_E|SEG_F|SEG_M|SEG_S,	// 0x44 'D'
	SEG_A|SEG_B|SEG_E|SEG_F|SEG_G|SEG_U|SEG_H,		// 0x45 'E'
	SEG_A|SEG_B|SEG_G|SEG_H|SEG_U|SEG_P,			// 0x46 'F'
	SEG_A|SEG_B|SEG_D|SEG_E|SEG_F|SEG_G|SEG_H|SEG_P,	// 0x47 'G'
	SEG_C|SEG_D|SEG_G|SEG_H|SEG_U|SEG_P,			// 0x48 'H'
	SEG_A|SEG_B|SEG_E|SEG_F|SEG_M|SEG_S,			// 0x49 'I'
	SEG_C|SEG_D|SEG_E|SEG_F|SEG_G,				// 0x4a 'J'
	SEG_N|SEG_R|SEG_H|SEG_U|SEG_G,				// 0x4b 'K'
	SEG_E|SEG_F|SEG_G|SEG_H,				// 0x4c 'L'
	SEG_C|SEG_D|SEG_G|SEG_H|SEG_K|SEG_N,			// 0x4d 'M'
	SEG_C|SEG_D|SEG_G|SEG_H|SEG_K|SEG_R,			// 0x4e 'N'
	SEG_A|SEG_B|SEG_C|SEG_D|SEG_E|SEG_F|SEG_G|SEG_H,	// 0x4f 'O'
	SEG_A|SEG_B|SEG_C|SEG_G|SEG_H|SEG_U|SEG_P,		// 0x50 'P'
	SEG_A|SEG_B|SEG_C|SEG_D|SEG_E|SEG_F|SEG_G|SEG_H|SEG_R,	// 0x51 'Q'
	SEG_A|SEG_B|SEG_C|SEG_G|SEG_H|SEG_P|SEG_R|SEG_U,	// 0x52 'R'
	SEG_A|SEG_B|SEG_D|SEG_E|SEG_F|SEG_H|SEG_U|SEG_P,	// 0x53 'S'
	SEG_A|SEG_B|SEG_M|SEG_S,				// 0x54 'T'
	SEG_C|SEG_D|SEG_E|SEG_F|SEG_G|SEG_H,			// 0x55 'U'
	SEG_H|SEG_G|SEG_T|SEG_M,				// 0x56 'V'
	SEG_C|SEG_D|SEG_H|SEG_G|SEG_T|SEG_R,			// 0x57 'W'
	SEG_K|SEG_N|SEG_R|SEG_T,				// 0x58 'X'
	SEG_S|SEG_K|SEG_N,					// 0x59 'Y'
	SEG_A|SEG_B|SEG_F|SEG_E|SEG_N|SEG_T,			// 0x5a 'Z'
	0x0000,	// 0x5b
	0x0000,	// 0x5c
	0x0000,	// 0x5d
	0x0000,	// 0x5e
	0x0000,	// 0x5f
	0x0000,	// 0x60
	0x0000,	// 0x61
	0x0000,	// 0x62
	0x0000,	// 0x63
	0x0000,	// 0x64
	0x0000,	// 0x65
	0x0000,	// 0x66
	0x0000,	// 0x67
	0x0000,	// 0x68
	0x0000,	// 0x69
	0x0000,	// 0x6a
	0x0000,	// 0x6b
	0x0000,	// 0x6c
	0x0000,	// 0x6d
	0x0000,	// 0x6e
	0x0000,	// 0x6f
	0x0000,	// 0x70
	0x0000,	// 0x71
	0x0000,	// 0x72
	0x0000,	// 0x73
	0x0000,	// 0x74
	0x0000,	// 0x75
	0x0000,	// 0x76
	0x0000,	// 0x77
	0x0000,	// 0x78
	0x0000,	// 0x79
	0x0000,	// 0x7a
	0x0000,	// 0x7b
	0x0000,	// 0x7c
	0x0000,	// 0x7d
	0x0000,	// 0x7e
	0x0000,	// 0x7f
};

#define KEY_STAR	10
#define KEY_POUND	11
#define KEY_TOP_RED	12
#define KEY_TOP_YELLOW	13
#define KEY_TOP_GREEN	14
#define KEY_BOT_YELLOW	15
#define KEY_WHITE	16

int decode_key(long long keycode) {

	int key=0;

	switch(keycode) {
		case 0x000001:	key=0;	break;
		case 0x000002:	key=1;	break;
		case 0x000004:	key=2;	break;
		case 0x000008:	key=3;	break;
		case 0x000010:	key=4;	break;
		case 0x000020:	key=5;	break;
		case 0x000040:	key=6;	break;
		case 0x000080:	key=7;  break;
		case 0x000100:	key=8;	break;
		case 0x000200:	key=9;	break;
		case 0x000400:	key=KEY_STAR; break;
		case 0x000800:	key=KEY_POUND;	break;
		case 0x010000:	key=KEY_TOP_RED; break;
		case 0x020000:	key=KEY_TOP_YELLOW; break;
		case 0x040000:	key=KEY_TOP_GREEN; break;
		case 0x080000:	key=KEY_BOT_YELLOW; break;
		case 0x100000:	key=KEY_WHITE; break;
		default:	printf("Unknown keycode %llx!\n",keycode);
				key=-1;	break;
	}
	return key;
}

#define FIELD_MONTH_1  0
#define FIELD_MONTH_2  1
#define FIELD_DATE_1   2
#define FIELD_DATE_2   3
#define FIELD_YEAR_1   4
#define FIELD_YEAR_2   5
#define FIELD_YEAR_3   6
#define FIELD_YEAR_4   7
#define FIELD_HOUR_1   8
#define FIELD_HOUR_2   9
#define FIELD_MIN_1   10
#define FIELD_MIN_2   11
#define FIELD_DONE    12

static long long old_keypad=0;
static int top_red=0,top_yellow=0,top_green=0,bot_yellow=0,white=0;



time_t keypad_change_time(int i2c_fd,long long keypad_in) {

	time_t delta_time=0,entered_time;
	struct tm new_time;
	int keypresses=0;
	int which_field=FIELD_MONTH_1,which_key=0;
	long long keypad_result,keypad_change;
	int done=0;
	int new_month=0;
	int new_date=0,new_year=0,new_hour=0,new_minute=0;

	while(1) {

		keypad_result=read_keypad(i2c_fd,HT16K33_ADDRESS1);

		keypad_change=old_keypad&~keypad_result;
		if (keypad_change) {
			which_key=decode_key(keypad_change);
			printf("Keypad: %d %d\n",keypresses,which_key);


			/* reset time if * or # pressed */
			if ((which_key==KEY_POUND || which_key==KEY_STAR)) {
				delta_time=0;
				return delta_time;

			}

			if (which_key==KEY_TOP_RED) {
				top_red=!top_red;
				return 0;
			}


			if (which_key==KEY_TOP_YELLOW) {
				top_yellow=!top_yellow;
				return 0;
			}


			if (which_key==KEY_TOP_GREEN) {
				top_green=!top_green;
				return 0;
			}

			if (which_key==KEY_BOT_YELLOW) {
				bot_yellow=!bot_yellow;
				return 0;
			}

			if (which_key==KEY_WHITE) {
				white=!white;
				return 0;
			}

			switch(which_field) {
				case FIELD_MONTH_1:
					if (which_key>1) {
						new_month=which_key;
						printf("Month now %d\n",new_month);
						which_field=FIELD_DATE_1;
					}
					else {
						new_month=which_key*10;
						which_field=FIELD_MONTH_2;
					}
					break;
				case FIELD_MONTH_2:
					new_month+=which_key;
					printf("Month now %d\n",new_month);
					which_field=FIELD_DATE_1;
					break;
				case FIELD_DATE_1:
					new_date=which_key*10;
					which_field=FIELD_DATE_2;
					break;
				case FIELD_DATE_2:
					new_date+=which_key;
					printf("Day now %d\n",new_date);
					which_field=FIELD_YEAR_1;
					break;
				case FIELD_YEAR_1:
					new_year=which_key*1000;
					which_field=FIELD_YEAR_2;
					break;
				case FIELD_YEAR_2:
					new_year+=which_key*100;
					which_field=FIELD_YEAR_3;
					break;
				case FIELD_YEAR_3:
					new_year+=which_key*10;
					which_field=FIELD_YEAR_4;
					break;
				case FIELD_YEAR_4:
					new_year+=which_key;
					printf("Year now %d\n",new_year);
					which_field=FIELD_HOUR_1;
					break;
				case FIELD_HOUR_1:
					new_hour=10*which_key;
					which_field=FIELD_HOUR_2;
					break;
				case FIELD_HOUR_2:
					new_hour+=which_key;
					printf("Hour now %d\n",new_hour);
					which_field=FIELD_MIN_1;
					break;
				case FIELD_MIN_1:
					new_minute=10*which_key;
					which_field=FIELD_MIN_2;
					break;
				case FIELD_MIN_2:
					new_minute+=which_key;
					printf("Minutes now %d\n",new_minute);
					which_field=FIELD_DONE;
					break;

			}

			keypresses++;
		}
		old_keypad=keypad_result;

		if (which_field==FIELD_DONE) done=1;
		if (done) break;

		usleep(20000);
	}

	new_time.tm_year=new_year-1900;
	new_time.tm_mon=new_month-1;
	new_time.tm_mday=new_date;
	new_time.tm_hour=new_hour;
	new_time.tm_min=new_minute;
	new_time.tm_sec=0;
	new_time.tm_isdst=-1;

	entered_time=mktime(&new_time);

	if (entered_time==-1) {
		printf("Error\n");
	}

	delta_time=(time(NULL))-entered_time;

	return delta_time;
}

struct tcircuit {
	int month;
	int day;
	int year;
	int hour;
	int minutes;
	int seconds;
};

static char month_names[12][4]={
	"JAN", "FEB", "MAR", "APR", "MAY", "JUN",
	"JUL", "AUG", "SEP", "OCT", "NOV", "DEC",

};

void print_time(struct tcircuit *tctime, int blink) {

	printf("%s ",month_names[tctime->month]);
	printf("%02d %04d",tctime->day,tctime->year);
	if (tctime->hour>11) {
		printf("\'");
	}
	else {
		printf(".");
	}
	printf("%02d",tctime->hour);
	if (blink) {
		printf(":");
	}
	else {
		printf(" ");
	}
	printf("%02d\n",tctime->minutes);
}

void convert_time(struct tm *ltime, struct tcircuit *tctime) {
	tctime->month=ltime->tm_mon;
	tctime->day=ltime->tm_mday;
	tctime->year=ltime->tm_year+1900;
	tctime->hour=ltime->tm_hour;
	tctime->minutes=ltime->tm_min;
	tctime->seconds=ltime->tm_sec;
}

int main(int argc, char **argv) {

 	int result,pm,hour,blink=0,count=0;
	int display_missing=0;
	int red_missing=0,green_missing=0,yellow_missing=0,flux_missing=0;

 	unsigned short red_buffer[8];
 	unsigned short green_buffer[8];
 	unsigned short yellow_buffer[8];
	unsigned short flux_buffer[8];

	struct tcircuit red_time;
	struct tcircuit yellow_time;
	struct tcircuit green_time;

	struct tm *ltime;

	long long keypad_result=0,keypad_change;
	int i2c_fd;

	time_t current_time;
	time_t delta_time=0,display_time;

	char *ctime_result;

	/* open i2c bus */
	i2c_fd=init_i2c("/dev/i2c-1");
	if (i2c_fd < 0) {
		fprintf(stderr,"Error opening device!\n");
		display_missing=1;
	}

	if (display_missing) {

	}
	else {

		/* Init Red Display (top) */
		if (init_display(i2c_fd,HT16K33_ADDRESS1,13)) {
			fprintf(stderr,"Error opening display\n");
			red_missing=1;
		}

		/* Init Green Display (middle) */
		if (init_display(i2c_fd,HT16K33_ADDRESS2,13)) {
			fprintf(stderr,"Error opening display\n");
			green_missing=1;
		}

		/* Init Yellow Display (bottom) */
		if (init_display(i2c_fd,HT16K33_ADDRESS3,13)) {
			fprintf(stderr,"Error opening display\n");
			yellow_missing=1;
		}

		/* Init Flux Capacitor */
		if (init_display(i2c_fd,HT16K33_ADDRESS4,13)) {
			fprintf(stderr,"Error opening display\n");
			flux_missing=1;
		}
	}

/*
	red_buffer[0]=font_16seg['A'];
	red_buffer[1]=font_16seg['U'];
	red_buffer[2]=font_16seg['G'];
	red_buffer[3]=font_7seg[2]|(font_7seg[8]<<8);
	red_buffer[4]=font_7seg[1]|(font_7seg[5]<<8);
	red_buffer[5]=font_7seg[2]|(font_7seg[4]<<8);
	red_buffer[6]=font_7seg[0]|(font_7seg[1]<<8);
	red_buffer[7]=font_7seg[1]|(font_7seg[3]<<8);
*/
/*

	red_buffer[7]=0xffff;
	red_buffer[6]=0xffff;
	red_buffer[5]=0xffff;
	red_buffer[4]=0xffff;
	red_buffer[3]=0xffff;
	red_buffer[2]=0xffff;
	red_buffer[1]=0xffff;
	red_buffer[0]=0xffff;
        update_display(i2c_fd,HT16K33_ADDRESS1,red_buffer);
	return 0;
*/

	/* Time Loop */
	while(1) {

		current_time=time(NULL);
		//ctime_result=ctime(&current_time);
		display_time=current_time-delta_time;
		ctime_result=ctime(&display_time );

		ltime=localtime(&display_time);

		convert_time(ltime,&red_time);
		convert_time(ltime,&yellow_time);
		convert_time(ltime,&green_time);


		if (display_missing) {

			/* clear screen */
			printf("\x1b[2J\x1b[0;0H");

			/* red */
			printf("\x1b[31;1m");
			print_time(&red_time,blink);

			/* green */
			printf("\x1b[32;1m");
			print_time(&green_time,blink);

			/* yellow */
			printf("\x1b[33;1m");
			print_time(&yellow_time,blink);
		}

		else {

			/* Month */
			red_buffer[5]=font_16seg[toupper(ctime_result[4])];
			red_buffer[6]=font_16seg[toupper(ctime_result[5])];
			red_buffer[7]=font_16seg[toupper(ctime_result[6])];

			/* Date */
			red_buffer[0]=font_7seg[ctime_result[8]-'0'];
			red_buffer[1]=font_7seg[ctime_result[9]-'0'];

			/* Year */
			red_buffer[2]=font_7seg[ctime_result[20]-'0'];
			red_buffer[3]=font_7seg[ctime_result[21]-'0'];
			red_buffer[4]=font_7seg[ctime_result[22]-'0'];
			red_buffer[4]|=(font_7seg[ctime_result[23]-'0'])<<8;

			/* hours */
			red_buffer[3]|=(font_7seg[ctime_result[11]-'0'])<<8;
			red_buffer[2]|=(font_7seg[ctime_result[12]-'0'])<<8;

			hour=((ctime_result[11]-'0')*10)+(ctime_result[12]-'0');
			if (hour>11) pm=1;
			else pm=0;

			/* minutes */
			red_buffer[1]|=(font_7seg[ctime_result[14]-'0'])<<8;
			red_buffer[0]|=(font_7seg[ctime_result[15]-'0'])<<8;

			/* AM/PM */
			if (pm) {
				red_buffer[3]|=0x8000;
			}
			else {
				red_buffer[2]|=0x8000;
			}

			/* Blink */
			if (blink) {
				red_buffer[0]|=0x8000;
				red_buffer[1]|=0x8000;
			}

			/* buttons */
			if (top_red) red_buffer[1]|=0x0080;
			if (top_yellow) red_buffer[2]|=0x0080;
			if (top_green) red_buffer[3]|=0x0080;
			if (bot_yellow) red_buffer[4]|=0x0080;
			if (white) red_buffer[4]|=0x8000;

			keypad_result=read_keypad(i2c_fd,HT16K33_ADDRESS1);
//		if (keypad_result!=-1) {
//			printf("keypad: %lld\n",keypad_result);
//		}

			keypad_change=old_keypad&~keypad_result;
			if (keypad_change) {
				delta_time=keypad_change_time(i2c_fd,keypad_change);
			}
			old_keypad=keypad_result;

			if (!red_missing) {
				update_display(i2c_fd,HT16K33_ADDRESS1,red_buffer);
			}
			if (!green_missing) {
				update_display(i2c_fd,HT16K33_ADDRESS2,green_buffer);
			}
			if (!yellow_missing) {
				update_display(i2c_fd,HT16K33_ADDRESS3,yellow_buffer);
			}
			if (!flux_missing) {
				update_display(i2c_fd,HT16K33_ADDRESS4,flux_buffer);
			}
		}

		usleep(100000);
		count++;
		if (count==5) {
			blink=1;
		}
		if (count==10) {
			count=0;
			blink=0;
		}
	}

	return result;
}

