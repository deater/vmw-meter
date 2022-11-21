/* A simple clock appication.  */
/* Prints the time on the alphanum segments */
/* Puts UNIX time along the bargraphs segments */
/* Day of the week on the colored LEDs */
/* Occasionally the month/day/year is also displayed */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <ctype.h>
#include <errno.h>

#include "14seg_font.h"
#include "i2c_lib.h"

#include "default_device.h"

static int reverse_bits32(int v) {

	unsigned int s = 32;
	unsigned int mask = ~0;
	while ((s >>= 1) > 0) {
		mask ^= (mask << s);
		v = ((v >> s) & mask) | ((v << s) & ~mask);
	}

	return v;

}


int main(int argc, char **argv) {

	int i,meter_fd,display_present=1;
	unsigned short display_state[8];
	char string[256];
	time_t seconds;  /* Y2038 problem! */
	unsigned int reverse_seconds;

	int printing_date=0;

	struct tm *breakdown;

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

	display_present=1;
	meter_fd=init_i2c(DEFAULT_DEVICE);
	if (meter_fd < 0) {
		fprintf(stderr,"Error opening device %s: %s!\n",
			DEFAULT_DEVICE,strerror(errno));
		sleep(5);
		display_present=0;
	}

	if (display_present) {
		init_saa1064_display(meter_fd,SAA1064_ADDRESS1);
		init_saa1064_display(meter_fd,SAA1064_ADDRESS2);
		init_saa1064_display(meter_fd,SAA1064_ADDRESS3);
		init_saa1064_display(meter_fd,SAA1064_ADDRESS4);
	}

	/* clear display */
	for(i=0;i<8;i++) display_state[i]=0;

	if (display_present) {
		update_saa1064_display(meter_fd, SAA1064_ADDRESS1,
					display_state);
	}
	else {
		update_saa1064_ascii(display_state);
	}

	while(1) {
		seconds=time(NULL);

		reverse_seconds=reverse_bits32(seconds);

		/* binary clock around eddge */
		display_state[7]=(reverse_seconds>>16)&0xffff;
		display_state[6]=(seconds>>16)&0xffff;

		breakdown=localtime(&seconds);

		/* occasionally print the day / date / year */
		if (seconds%128==0) printing_date=1;
		else if (seconds%128==1) printing_date=2;
		else if (seconds%128==2) printing_date=3;
		else printing_date=0;

		switch(printing_date) {
		case 0:
			/* time display */
			display_state[5]=ascii_lookup[0x30 + (breakdown->tm_sec % 10)];
			display_state[4]=ascii_lookup[0x30 + (breakdown->tm_sec / 10)];

			display_state[3]=ascii_lookup[0x30 + (breakdown->tm_min % 10)];
			display_state[2]=ascii_lookup[0x30 + (breakdown->tm_min / 10)];
			display_state[3]|=SAA1064_SEGMENT_DP;

			display_state[1]=ascii_lookup[0x30 + (breakdown->tm_hour % 10)];
			display_state[0]=ascii_lookup[0x30 + (breakdown->tm_hour / 10)];
			display_state[1]|=SAA1064_SEGMENT_DP;
			break;

		case 1:
			strftime(string,4,"%a",breakdown);
			display_state[0]=ascii_lookup[toupper(string[0])];
			display_state[1]=ascii_lookup[toupper(string[1])];
			display_state[2]=ascii_lookup[toupper(string[2])];
			display_state[3]=0;
			display_state[4]=0;
			display_state[5]=0;

			break;
		case 2:
			strftime(string,8,"%b %d",breakdown);
			display_state[0]=ascii_lookup[toupper(string[0])];
			display_state[1]=ascii_lookup[toupper(string[1])];
			display_state[2]=ascii_lookup[toupper(string[2])];
			display_state[3]=ascii_lookup[toupper(string[3])];
			display_state[4]=ascii_lookup[toupper(string[4])];
			display_state[5]=ascii_lookup[toupper(string[5])];
			break;
		case 3:
			strftime(string,8,"%_6Y",breakdown);
			display_state[0]=ascii_lookup[(int)string[0]];
			display_state[1]=ascii_lookup[(int)string[1]];
			display_state[2]=ascii_lookup[(int)string[2]];
			display_state[3]=ascii_lookup[(int)string[3]];
			display_state[4]=ascii_lookup[(int)string[4]];
			display_state[5]=ascii_lookup[(int)string[5]];
			break;
		}

		/* day of week (Sunday is none) */
		if (breakdown->tm_wday>0) {
			display_state[breakdown->tm_wday - 1]|=SAA1064_SEGMENT_EX;
		}

		if (display_present) {
			update_saa1064_display(meter_fd,
				SAA1064_ADDRESS1, display_state);
		}
		else {
			update_saa1064_ascii(display_state);
		}

		usleep(200000);
	}

	return 0;
}


