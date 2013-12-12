/* This tool shows the potential for the SVMW Meter as a gaming */
/* accessory in conunction with space games */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include "14seg_font.h"
#include "i2c_lib.h"

static unsigned short display_state[8];
static int left_shields=0,right_shields=0;
static int meter_fd;

static void set_shields(int left, int right) {

	/* 0x0000 0x8000 0xc000 0xe000 0xf000 ...*/
	//  display_state[6]&=~0xffc0;
	display_state[6]=~((1<<(16-left))-1);
	display_state[7]=~((1<<(16-right))-1);
}

static void set_string(char *string) {

	int i;

	/* set string */
	for(i=0;i<6;i++) {
		if (i<strlen(string)) {
			display_state[i]=ascii_lookup[(unsigned char)string[i]];
		}
		else {
			display_state[i]=0;
		}
	}
}

static void update_our_display(int display_present) {

	if (display_present) {
		update_saa1064_display(meter_fd, SAA1064_ADDRESS1,
			display_state);
	}
	else {
		update_saa1064_ascii(display_state);
	}

}

int main(int argc, char **argv) {

	int i;
	int display_present;

	display_present=1;
	meter_fd=init_i2c("/dev/i2c-6");
	if (meter_fd < 0) {
		fprintf(stderr,"Error opening device!\n");
		display_present=0;
	}

	for(i=0;i<8;i++) display_state[i]=0;

	update_our_display(display_present);

	while(1) {

		/* ALL OK */

		set_string("ALL OK");
		display_state[3]|=SAA1064_SEGMENT_EX;
		update_our_display(display_present);

		sleep(5);


		/* ENEMY ALERT */
		set_string("ENEMY");
		display_state[3]|=SAA1064_SEGMENT_EX;
		update_our_display(display_present);
		sleep(1);

		set_string("ALERT");
		display_state[3]|=SAA1064_SEGMENT_EX;
		update_our_display(display_present);
		sleep(1);

		/* SHIELDS UP */
		set_string("SHLDUP");
		display_state[3]|=SAA1064_SEGMENT_EX;
		update_our_display(display_present);

		for(i=0;i<11;i++) {
			set_shields(i,i);
			update_our_display(display_present);
			usleep(200000);
		}
		left_shields=10;
		right_shields=10;

		/* UNDER ATTACK */

		set_string("UNDER");
		display_state[3]|=SAA1064_SEGMENT_EX;
		update_our_display(display_present);
		sleep(1);

		set_string("ATTACK");
		display_state[3]|=SAA1064_SEGMENT_EX;
		update_our_display(display_present);
		sleep(2);

		/* YELLOW ALERT */
		set_string("YELLOW");
		display_state[2]|=SAA1064_SEGMENT_EX;
		update_our_display(display_present);
		sleep(1);

		set_string("ALERT");
		display_state[2]|=SAA1064_SEGMENT_EX;
		update_our_display(display_present);
		sleep(3);


		/* HIT */
		set_string("ENEMY");
		display_state[0]|=SAA1064_SEGMENT_EX;
		display_state[1]|=SAA1064_SEGMENT_EX;
		display_state[2]|=SAA1064_SEGMENT_EX;
		display_state[3]|=SAA1064_SEGMENT_EX;
		display_state[4]|=SAA1064_SEGMENT_EX;
		display_state[5]|=SAA1064_SEGMENT_EX;
		left_shields=8;
		right_shields=7;
		set_shields(left_shields,right_shields);
		update_our_display(display_present);
		sleep(2);

		set_string("*HIT*");
		display_state[2]|=SAA1064_SEGMENT_EX;
		update_our_display(display_present);
		sleep(2);


		set_string(" ");
		display_state[2]|=SAA1064_SEGMENT_EX;
		update_our_display(display_present);
		sleep(2);


		/* *FIRE* */
		set_string("*FIRE*");
		display_state[2]|=SAA1064_SEGMENT_EX;
		update_our_display(display_present);
		sleep(1);

		set_string("LASERS");
		display_state[2]|=SAA1064_SEGMENT_EX;
		update_our_display(display_present);
		sleep(1);

		for(i=0;i<6;i++) {
			display_state[6]=(display_state[6]&~0x3f)|1<<(5-i);
			display_state[7]=(display_state[7]&~0x3f)|1<<(5-i);
			update_our_display(display_present);
			usleep(200000);
		}

		set_string("-MISS-");
		display_state[2]|=SAA1064_SEGMENT_EX;
		display_state[6]=(display_state[6]&~0x3f);
		display_state[7]=(display_state[7]&~0x3f);
		update_our_display(display_present);
		sleep(2);

		/* Enemy Hit 2 */
		set_string("ENEMY");
		display_state[0]|=SAA1064_SEGMENT_EX;
		display_state[1]|=SAA1064_SEGMENT_EX;
		display_state[2]|=SAA1064_SEGMENT_EX;
		display_state[3]|=SAA1064_SEGMENT_EX;
		display_state[4]|=SAA1064_SEGMENT_EX;
		display_state[5]|=SAA1064_SEGMENT_EX;
		left_shields=5;
		right_shields=4;
		set_shields(left_shields,right_shields);
		update_our_display(display_present);
		sleep(2);

		set_string("*HIT*");
		display_state[2]|=SAA1064_SEGMENT_EX;
		update_our_display(display_present);
		sleep(2);

		/* LASERS BROKEN */

		set_string("LASERS");
		display_state[2]|=SAA1064_SEGMENT_EX;
		update_our_display(display_present);
		sleep(2);

		set_string("BROKEN");
		display_state[2]|=SAA1064_SEGMENT_EX;
		update_our_display(display_present);
		sleep(2);

		/* SHLDS < 50% */

		set_string("SHLDS");
		display_state[2]|=SAA1064_SEGMENT_EX;
		update_our_display(display_present);
		sleep(1);

		set_string("< 50% ");
		display_state[2]|=SAA1064_SEGMENT_EX;
		update_our_display(display_present);
		sleep(1);

		/* RED ALERT */

		set_string("RED");
		display_state[0]|=SAA1064_SEGMENT_EX;
		update_our_display(display_present);
		sleep(1);

		set_string("ALERT");
		display_state[0]|=SAA1064_SEGMENT_EX;
		update_our_display(display_present);
		sleep(2);

		/* Enemy Hit 3 */
		set_string("ENEMY");
		display_state[0]|=SAA1064_SEGMENT_EX;
		display_state[1]|=SAA1064_SEGMENT_EX;
		display_state[2]|=SAA1064_SEGMENT_EX;
		display_state[3]|=SAA1064_SEGMENT_EX;
		display_state[4]|=SAA1064_SEGMENT_EX;
		display_state[5]|=SAA1064_SEGMENT_EX;
		left_shields=1;
		right_shields=2;
		set_shields(left_shields,right_shields);
		update_our_display(display_present);
		sleep(2);

		set_string("*HIT*");
		display_state[0]|=SAA1064_SEGMENT_EX;
		update_our_display(display_present);
		sleep(2);

		/* DANGER HULL BREACH DECK 7 */
		set_string("HULL");
		display_state[0]|=SAA1064_SEGMENT_EX;
		update_our_display(display_present);
		sleep(1);

		set_string("BREACH");
		display_state[0]|=SAA1064_SEGMENT_EX;
		update_our_display(display_present);
		sleep(1);

		set_string("DECK 7");
		display_state[0]|=SAA1064_SEGMENT_EX;
		update_our_display(display_present);
		sleep(2);


		/* Enemy Hit 4 */
		set_string("ENEMY");
		display_state[0]|=SAA1064_SEGMENT_EX;
		display_state[1]|=SAA1064_SEGMENT_EX;
		display_state[2]|=SAA1064_SEGMENT_EX;
		display_state[3]|=SAA1064_SEGMENT_EX;
		display_state[4]|=SAA1064_SEGMENT_EX;
		display_state[5]|=SAA1064_SEGMENT_EX;
		left_shields=0;
		right_shields=1;
		set_shields(left_shields,right_shields);
		update_our_display(display_present);
		sleep(2);

		set_string("*HIT*");
		display_state[0]|=SAA1064_SEGMENT_EX;
		update_our_display(display_present);
		sleep(2);

		/* DANGER WARP CORE FAIL */

		set_string("WARP");
		display_state[0]|=SAA1064_SEGMENT_EX;
		update_our_display(display_present);
		sleep(2);

		set_string("CORE");
		display_state[0]|=SAA1064_SEGMENT_EX;
		update_our_display(display_present);
		sleep(2);

		set_string("FAIL");
		display_state[0]|=SAA1064_SEGMENT_EX;
		update_our_display(display_present);
		sleep(3);


		/* EXPLOSION */

		for(i=0;i<8;i++) {
			display_state[i]=0xffff;
		}
		update_our_display(display_present);
		sleep(5);

		for(i=0;i<8;i++) {
			display_state[i]=0;
		}

		update_our_display(display_present);
		sleep(5);

		/* LOOP */

	}

	return 0;
}


