#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include <signal.h>

#include <sys/time.h>

#include <sys/ioctl.h>
#include <sys/resource.h>
#include <linux/i2c-dev.h>

#include "display.h"
#include "i2c_lib.h"

#include "ay-3-8910.h"
#include "ym_lib.h"
#include "max98306.h"

#include <bcm2835.h>

#include "14seg_font.h"

#include "lyrics.h"
#include "glados_ascii_art.h"

static int display_type=DISPLAY_I2C;
static int play_music=1;
static int shift_size=16;

static int y_line=0;


static void quiet_and_exit(int sig) {

	if (play_music) {
		quiet_ay_3_8910(shift_size);
		close_ay_3_8910();
		max98306_free();
	}

	display_shutdown(display_type);

	printf("Quieting and exiting\n");
	_exit(0);

}


static int clear_things(int side_too) {

	int i;
	char string[BUFSIZ];

	if (side_too) {
		/* clear screen */
		write(1,"\033[2J",4);

		/* clear 16x8 display */
		display_led_art(display_type,led_art,1024);
	}

	write(1,"\033[1;1H--------------------------------------",44);
	for(i=2;i<24;i++) {
		sprintf(string,"\033[%d;1H|                                    |",i);
		write(1,string,strlen(string));
	}
	write(1,"\033[24;1H--------------------------------------",45);
	write(1,"\033[2;2H",6);
	y_line=2;
	return 0;
}

static void print_ascii_art(int which) {
	int i;
	char string[BUFSIZ];

	/* save cursor position */
	write(1,"\033[s",3);

	for(i=0;i<21;i++) {
		sprintf(string,"\033[%d;40H",i+2);
		write(1,string,strlen(string));
		write(1,ascii_art[which][i],strlen(&ascii_art[which][i][0]));
		write(1," \033[K",4);
	}

	/* restore cursor position */
	write(1,"\033[u",3);

	return;
}


/* Currently we are 8 50Hz interrupts per 16th note */
#define MAX_LYRIC_LEN	8


static int ignore_led=0;

static int parse_lyric(struct lyric_type *l, int lnum, char *string) {

	int length=0,sub=0;
	int ch;

	while(1) {

		ch=l->l[lnum].text[sub];

		if (ch==0) break;

		/* Handle special escape characters */
		if (ch=='\\') {
			sub++;
			ch=l->l[lnum].text[sub];

			/* \i means don't write text to LED display */
			if (ch=='i') {
				ignore_led=1;
			}

			/* \n is special, we delay updating LED */
			if (ch=='n') {
				string[length]='\n';
				length++;
				if (length>MAX_LYRIC_LEN) break;
			}

			/* \1 - \: (i.e. 1-10) */
			/* Update ASCII art on screen and 8x16 panel */
			if ((ch>='1')&&(ch<=':')) {
				print_ascii_art(ch-'1');
				display_led_art(display_type,led_art,ch-'1');
			}

			/* \f means clear screen */
			if (ch=='f') {
				string[length]='\f';
				length++;
				if (length>MAX_LYRIC_LEN) break;
			}
		}

		else {
			string[length]=ch;
			length++;
			if (length>MAX_LYRIC_LEN) break;
		}

		sub++;

	}

	if (length>MAX_LYRIC_LEN) {
		fprintf(stderr,"ERROR! LYRIC TOO LONG!\n");
		return -1;
	}

	/* NUL terminate */
	string[length]=0;

	return length;
}

static int lyrics_play(struct lyric_type *l) {

	int frame=0,lnum=0,sub=0;
	char led_string[NUM_ALPHANUM],ch;
	int i;
	char string[BUFSIZ];
	int result;
	int led_offset=0,clear_next=0;

	struct timeval start,next;
	double s,n,diff;

	struct ym_song_t ym_song;

	int lyric_active=0;
	char current_lyric[MAX_LYRIC_LEN+1];
	struct frame_stats ds;

	result=load_ym_song("sa/sa.ym5",&ym_song);
	if (result<0) {
		return -1;
	}

	clear_things(1);

        gettimeofday(&start,NULL);

	frame=0;
	while(1) {

		/* Play the music for this frame */
		ym_play_frame(&ym_song,frame,shift_size,
				&ds,0,play_music,0);

		/* Update the bargraph */
		bargraph(display_type,ds.a_bar,ds.b_bar,ds.c_bar);

		/* Parse any lyric updates for this frame */

		/* We cross a lyric threshold, start a lyric */
		if (frame==l->l[lnum].frame) {
			lyric_active=1;
			sub=0;
			ignore_led=0;
			parse_lyric(l,lnum,current_lyric);
		}

		if (lyric_active) {
			ch=current_lyric[sub];
			if (ch==0) {
				lyric_active=0;
				lnum++;
			}
			else if (ch=='\n') {
				y_line++;
				sprintf(string,"\n\033[%d;2H",y_line);
				write(1,string,strlen(string));

				if (!ignore_led) clear_next=1;
			}
			else if (ch=='\f') {
				clear_things(0);
				y_line=2;

				for(i=0;i<NUM_ALPHANUM;i++) {
					led_string[i]=' ';
				}
				led_offset=0;
			}
			else {
				write(1,&ch,1);

				if (!ignore_led) {
					if (clear_next) {
						for(i=0;i<NUM_ALPHANUM;i++) {
							led_string[i]=' ';
						}
						led_offset=0;
						clear_next=0;
					}

					if (led_offset<NUM_ALPHANUM) {

					}
					else {
						for(i=1;i<NUM_ALPHANUM;i++) {
							led_string[i-1]=led_string[i];
						}
						led_offset=NUM_ALPHANUM-1;
					}

					if (isalpha(ch)) {
						led_string[led_offset]=toupper(ch);
					}
					else {
						led_string[led_offset]=ch;
					}
					led_offset++;
				}
			}

			if (display_type==DISPLAY_I2C) {
				display_string(display_type,led_string);
			}
			sub++;
		}



		/* Calculate time we were busy this frame */
		gettimeofday(&next,NULL);
		s=start.tv_sec+(start.tv_usec/1000000.0);
		n=next.tv_sec+(next.tv_usec/1000000.0);
		diff=(n-s)*1000000.0;

		start.tv_sec=next.tv_sec;
		start.tv_usec=next.tv_usec;

		/* Delay until time for next update (often 50Hz) */
                if (play_music) {
                        if (diff>0) bcm2835_delayMicroseconds(20000-diff);
                        /* often 50Hz = 20000 */
                        /* TODO: calculate correctly */
                }
                else {
                        if (1) usleep(1000000/ym_song.frame_rate);
                }


		frame++;

		/* If hit the end of the song, then stop */
		if (frame>ym_song.num_frames) break;
	}

	return 0;

}

int main(int argc, char **argv) {

	int result;

	struct lyric_type l;

	/* Setup control-C handler to quiet the music   */
	/* otherwise if you force quit it keeps playing */
	/* the last tones */
	signal(SIGINT, quiet_and_exit);

	/* Set to have highest possible priority */
	setpriority(PRIO_PROCESS, 0, -20);

	if (play_music) {
		result=initialize_ay_3_8910(1);
		if (result<0) {
			printf("Error initializing bcm2835!!\n");
			play_music=0;
		}
		result=max98306_init();
		if (result<0) {
			printf("Error initializing max98306 amp!!\n");
			play_music=0;
		}
		else {
			result=max98306_enable();
		}
	}

	result=display_init(display_type);

//	translate_to_adafruit();

//	display_led_art(6);

	load_lyrics("sa/sa.lyrics",&l);

	lyrics_play(&l);

	destroy_lyrics(&l);

	if (play_music) {
		quiet_ay_3_8910(shift_size);
		close_ay_3_8910();
		max98306_free();
	}

	display_shutdown(display_type);

	return 0;
}
