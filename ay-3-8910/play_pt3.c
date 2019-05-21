/* Parse/Play a YM AY-3-8910 Music File */
/* Used file info found here: http://leonard.oxg.free.fr/ymformat.html */
/* Also useful: ftp://ftp.modland.com/pub/documents/format_documentation/Atari%20ST%20Sound%20Chip%20Emulator%20YM1-6%20(.ay,%20.ym).txt */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <math.h>
#include <sys/resource.h>

#include <bcm2835.h>

#include "ymlib/stats.h"
#include "ay-3-8910.h"
#include "display.h"
#include "pt3_lib.h"
#include "max98306.h"
#include "visualizations.h"

#include "version.h"

//#define AY38910_CLOCK	1000000	/* 1MHz on our board */

static int play_music=1;
static int mute_channel=0;
static int dump_info=0;
static int visualize=1;
static int display_type=DISPLAY_I2C;
static int shift_size=16;
static int music_repeat=0;
static int music_paused=0;
static int music_loop=0;
static int diff_mode=0;
static int volume=1;
static int amp_disable=0;

#define YM5_FRAME_SIZE	16

int ym_play_frame(unsigned char *frame, int shift_size,
			struct display_stats *ds,
			int diff_mode,
			int play_music,
			int mute_channel) {

	int j;

	unsigned char frame2[YM5_FRAME_SIZE];

	int left_a_period,left_b_period,left_c_period;
	int right_a_period,right_b_period,right_c_period;

	double left_a_freq=0.0, left_b_freq=0.0, left_c_freq=0.0;
	double right_a_freq=0.0, right_b_freq=0.0, right_c_freq=0.0;

	int channels=3;
	int master_clock=1777000;

	if (channels==3) {
		memcpy(frame2,frame,sizeof(frame2));
	} else {
//		ym_make_frame(ym_song,ym_song->frame_data2,frame_num,frame2,0);
	}

	left_a_period=((frame[1]&0xf)<<8)|frame[0];
	left_b_period=((frame[3]&0xf)<<8)|frame[2];
	left_c_period=((frame[5]&0xf)<<8)|frame[4];

	right_a_period=((frame2[1]&0xf)<<8)|frame2[0];
	right_b_period=((frame2[3]&0xf)<<8)|frame2[2];
	right_c_period=((frame2[5]&0xf)<<8)|frame2[4];

	if (left_a_period>0) left_a_freq=master_clock/(16.0*(double)left_a_period);
	if (left_b_period>0) left_b_freq=master_clock/(16.0*(double)left_b_period);
	if (left_c_period>0) left_c_freq=master_clock/(16.0*(double)left_c_period);

//	if (left_n_period>0) left_n_freq=master_clock/(16.0*(double)left_n_period);

	if (right_a_period>0) right_a_freq=master_clock/(16.0*(double)right_a_period);
	if (right_b_period>0) right_b_freq=master_clock/(16.0*(double)right_b_period);
	if (right_c_period>0) right_c_freq=master_clock/(16.0*(double)right_c_period);

//	if (right_n_period>0) right_n_freq=master_clock/(16.0*(double)left_n_period);



	if (mute_channel&0x1) frame[8]=0;
	if (mute_channel&0x2) frame[9]=0;
	if (mute_channel&0x4) frame[10]=0;
	if (mute_channel&0x8) frame[7]|=0x8;
	if (mute_channel&0x10) frame[7]|=0x10;
	if (mute_channel&0x20) frame[7]|=0x20;
	/* Mute envelope */
	if (mute_channel&0x40) {
		frame[8]&=0xf;
		frame[9]&=0xf;
		frame[10]&=0xf;
	}

	/* FIXME: do this right? */
	if (mute_channel&0x1) frame2[8]=0;
	if (mute_channel&0x2) frame2[9]=0;
	if (mute_channel&0x4) frame2[10]=0;
	if (mute_channel&0x8) frame2[7]|=0x8;
	if (mute_channel&0x10) frame2[7]|=0x10;
	if (mute_channel&0x20) frame2[7]|=0x20;
	/* Mute envelope */
	if (mute_channel&0x40) {
		frame2[8]&=0xf;
		frame2[9]&=0xf;
		frame2[10]&=0xf;
	}

	if (play_music) {
		for(j=0;j<13;j++) {
			write_ay_3_8910(j,frame[j],frame2[j],shift_size);
		}

		/* Special case.  Writing r13 resets it,	*/
		/* so special 0xff marker means do not write	*/

		/* FIXME: so what do we do if 2 channels have */
		/* different values? */
		/* We'll have to special case, and do a dummy write */
		/* to a non-13 address.  Should be possible but not */
		/* worth fixing unless it actually becomes a problem. */


		if ((frame[13]!=0xff) || (frame2[13]!=0xff)) {
			write_ay_3_8910(13,frame[13],frame2[13],shift_size);
		}
	}

	if (ds!=NULL) {
		ds->left_amplitude[0]=frame[8];
		ds->left_amplitude[1]=frame[9];
		ds->left_amplitude[2]=frame[10];

		ds->right_amplitude[0]=frame2[8];
		ds->right_amplitude[1]=frame2[9];
		ds->right_amplitude[2]=frame2[10];

		ds->left_freq[0]=left_a_freq;
		ds->left_freq[1]=left_b_freq;
		ds->left_freq[2]=left_c_freq;

		ds->right_freq[0]=right_a_freq;
		ds->right_freq[1]=right_b_freq;
		ds->right_freq[2]=right_c_freq;
	}

	return 0;

}


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

static void print_help(int just_version, char *exec_name) {

	printf("\nym_play version %s by Vince Weaver <vince@deater.net>\n\n",VERSION);
	if (just_version) exit(0);

	printf("This plays YM5 music files out of the VMW AY-3-8910 device\n");
	printf("on a suitably configured Raspberry Pi machine.\n\n");

	printf("Usage:\n");
	printf("\t%s [-h] [-v] [-d] [-i] [-t] [-m] [-s] [-n] filename\n\n",
		exec_name);
	printf("\t-h: this help message\n");
	printf("\t-v: version info\n");
	printf("\t-d: print debug messages\n");
	printf("\t-D: debug diff mode\n");
	printf("\t-i: use i2c LEDs for visualization\n");
	printf("\t-t: use ASCII text visualization\n");
	printf("\t-m: use mono (only one AY-3-8910 hooked up)\n");
	printf("\t-s: use stereo (two AY-3-8910s hooked up)\n");
	printf("\t-n: no sound (useful when debugging)\n");
	// printf("\t-k: kiosk mode (standalone without keyboard/monitor)\n");
	//printf("\tfilename: must be uncompressed YM5 file for now\n\n");

	exit(0);
}


static int current_mode=MODE_VISUAL;

static int handle_keypress(void) {

	int display_command=0;

	/* Handle keypresses */
	do {
		display_command=display_keypad_read(display_type);

		if (display_command==CMD_MUTE_A) {
			if (mute_channel&0x01) mute_channel&=~0x01;
			else mute_channel|=0x01;
			printf("NEW %x\n",mute_channel);
		}
		if (display_command==CMD_MUTE_B) {
			if (mute_channel&0x2) mute_channel&=~0x02;
			else mute_channel|=0x02;
			printf("NEW %x\n",mute_channel);
		}
		if (display_command==CMD_MUTE_C) {
			if (mute_channel&0x04) mute_channel&=~0x04;
			else mute_channel|=0x04;
			printf("NEW %x\n",mute_channel);
		}
		if (display_command==CMD_MUTE_NA) {
			if (mute_channel&0x08) mute_channel&=~0x08;
			else mute_channel|=0x08;
			printf("NEW %x\n",mute_channel);
		}
		if (display_command==CMD_MUTE_NB) {
			if (mute_channel&0x10) mute_channel&=~0x10;
			else mute_channel|=0x10;
			printf("NEW %x\n",mute_channel);
		}
		if (display_command==CMD_MUTE_NC) {
			if (mute_channel&0x20) mute_channel&=~0x20;
			else mute_channel|=0x20;
			printf("NEW %x\n",mute_channel);
		}
		if (display_command==CMD_MUTE_ENVELOPE) {
			if (mute_channel&0x40) mute_channel&=~0x40;
			else mute_channel|=0x40;
			printf("NEW %x\n",mute_channel);
		}

		if (display_command==CMD_EXIT_PROGRAM) {
			return CMD_EXIT_PROGRAM;
		}

		/* prev song */
		if (display_command==CMD_BACK) {
			return CMD_BACK;
		}
		/* next song */
		if (display_command==CMD_NEXT) {
			return CMD_NEXT;
		}

		/* rewind = Beginning of track */
		if (display_command==CMD_RW) {
//			frame_num=0;
		}

		/* fastfwd = skip ahead 5s */
		if (display_command==CMD_FF) {
//			frame_num+=5*pt3_song.frame_rate;
		}

		if (display_command==CMD_PLAY) {
			if (music_paused) {
				music_paused=0;
				max98306_enable();
			}
			else {
				music_paused=1;
				quiet_ay_3_8910(shift_size);
				max98306_disable();
			}
		}

		if (display_command==CMD_STOP) {
			if (!music_paused) {
				music_paused=1;
				quiet_ay_3_8910(shift_size);
				max98306_disable();
			}
		}

		if (display_command==CMD_VOL_UP) {
			volume++;
			if (volume>5) volume=5;
			max98306_set_volume(volume);
		}

		if (display_command==CMD_VOL_DOWN) {
			volume--;
			if (volume<0) volume=0;
			max98306_set_volume(volume);
		}

		if (display_command==CMD_AMP_OFF) {
			if (play_music) max98306_disable();
		}

		if (display_command==CMD_AMP_ON) {
			if (play_music) max98306_enable();
		}


		if (display_command==CMD_MENU) {
			current_mode++;
			if (current_mode>=MODE_MAX) current_mode=0;
		}


		if (display_command==CMD_LOOP) {
			music_loop=!music_loop;
			if (music_loop) printf("MUSIC LOOP ON\n");
			else printf("MUSIC LOOP OFF\n");
		}
		/* Avoid spinning CPU if paused */
		if (music_paused) usleep(100000);
	} while (music_paused);
	return 0;
}

#define TEXT_MODE_BANNER	0
#define TEXT_MODE_FILENAME	1
#define TEXT_MODE_SONGNAME	2
#define TEXT_MODE_AUTHOR	3
#define TEXT_MODE_TIMER		4
#define TEXT_MODE_MENU		5
#define TEXT_MODE_TRACKER	6


static char display_text[13];

static void music_visualize(int frames_elapsed, int frame_num, char *filename,
		int length_seconds, char *name, char *author,
		char *string) {

	static int text_mode=TEXT_MODE_BANNER;

	static int state_count=0;
	static int string_pointer=0;
	static int scroll_rate=0;
	char full_text[BUFSIZ];

	int k;

	/********************************************/
	/* Update the alphanum display statemachine */
	/********************************************/

	if (frames_elapsed==150) {

		text_mode=TEXT_MODE_FILENAME;
		memset(display_text,0,13);

		/* strip off path info */
		k=strlen(filename)-1;
		while(k>0) {
			if (filename[k-1]=='/') break;
			k--;
		}
		strcpy(full_text,filename+k);
		snprintf(display_text,13,full_text);
		state_count=0;
		string_pointer=0;
		scroll_rate=0;
		if (strlen(full_text)>12) {
			scroll_rate=100/(strlen(full_text)-12);
		}
	} else if (frames_elapsed==300) {
		text_mode=TEXT_MODE_SONGNAME;
		memset(display_text,0,13);
		strcpy(full_text,name);
		snprintf(display_text,13,full_text);
		state_count=0;
		string_pointer=0;
		scroll_rate=0;
		if (strlen(full_text)>12) {
			scroll_rate=100/(strlen(full_text)-12);
		}
	} else if (frames_elapsed==450) {
		text_mode=TEXT_MODE_AUTHOR;
		memset(display_text,0,13);
		sprintf(full_text,"BY: %s",author);

		snprintf(display_text,13,full_text);
		state_count=0;
		string_pointer=0;
		scroll_rate=0;
		if (strlen(full_text)>12) {
			scroll_rate=100/(strlen(full_text)-12);
		}
	} else if (frames_elapsed==600) {
		text_mode=TEXT_MODE_TRACKER;
	}



	switch(text_mode) {
		case TEXT_MODE_BANNER:
			/* do nothing, banner already displayed */
			break;
		case TEXT_MODE_FILENAME:
		case TEXT_MODE_SONGNAME:
		case TEXT_MODE_AUTHOR:
			/* have 3s (150 frames) */
			if (state_count<25) {
				/* do nothing, show beginning for a bit */
				/* to give time to read */
			}
			else if (state_count<125) {
				if (scroll_rate) {
					if ((state_count-25)%scroll_rate==0) {
						string_pointer++;
						snprintf(display_text,13,full_text+string_pointer);
					}
				}
			}
			break;
		case TEXT_MODE_TIMER:
			if (frames_elapsed%25==0) {
				memset(display_text,0,13);
				snprintf(display_text,13,"%2d:%02d--%2d:%02d",
					(frame_num/50)/60,(frame_num/50)%60,
					length_seconds/60,length_seconds%60);
			}
			break;
		case TEXT_MODE_TRACKER:
			memset(display_text,0,13);
			snprintf(display_text,13,string);
			break;
		default:
			break;
	}

	display_14seg_string(display_type,display_text);
	state_count++;

	return;
}




/* TIMING */
/* for 50Hz have a 20ms deadline */
/* Currently (with GPIO music and Linux i2c):			   */
/*   ym_play_frame:        14ms (need to move to SPI!)             */
/*   display_update:        1-3ms (depends on if all are updated)  */
/*   display_keypad_read: 0.5ms                                    */
/*   display_string:        5ms (slow!  3 i2c addresses            */
/*				5.02ms (Linux) 4.9ms (libbcm)	   */



static int frame_rate=50;

static int play_song(char *filename) {

	int i,j,f,length_seconds;
	double s,n,hz,diff;
	int temp_scale;

	int result;
	int frame_num=0,frames_elapsed=0;

	struct timeval start,next;

	struct pt3_song_t pt3,pt3_2;
	struct display_stats ds;
	char string[13];

	unsigned char frame[16];

#define TIMING_DEBUG	0

#if TIMING_DEBUG==1
	struct timeval before,after;
	double b,a;
	FILE *debug_file;
	debug_file=fopen("timing.debug","w");
#endif

	printf("\nPlaying song %s\n",filename);

	result=pt3_load_song(filename,&pt3,&pt3_2);
	if (result<0) {
		return -1;
	}

	gettimeofday(&start,NULL);

	/**********************/
	/* Print song summary */
	/**********************/

	length_seconds=60;
	printf("\tSong name: %s\n",pt3.name);
	printf("\tAuthor name: %s\n",pt3.author);

	sprintf(display_text,"VMW CHIPTUNE");
	display_14seg_string(display_type,display_text);

	/******************/
	/* Play the song! */
	/******************/

	frame_num=0;

	for(i=0;i < pt3.music_len;i++) {

		pt3_set_pattern(i,&pt3);

		for(j=0;j<64;j++) {

			/* decode line. 1 if done early */
			if (pt3_decode_line(&pt3)) break;

                        /* Dump out subframes of line */
                        for(f=0;f<pt3.speed;f++) {

				pt3_make_frame(&pt3,frame);

				/* pt3 files typically assume 1.77MHz! */

				/* A */
				temp_scale=frame[0]|(frame[1]<<8);
				temp_scale=(temp_scale*9)/16;
				frame[0]=(temp_scale&0xff);
				frame[1]=(temp_scale>>8)&0xf;

				/* B */
				temp_scale=frame[2]|(frame[3]<<8);
				temp_scale=(temp_scale*9)/16;
				frame[2]=(temp_scale&0xff);
				frame[3]=(temp_scale>>8)&0xf;

				/* C */
				temp_scale=frame[4]|(frame[5]<<8);
				temp_scale=(temp_scale*9)/16;
				frame[4]=(temp_scale&0xff);
				frame[5]=(temp_scale>>8)&0xf;

				/* N */
				temp_scale=frame[6];
				temp_scale=(temp_scale*9)/16;
				frame[6]=(temp_scale&0x1f);

				/* E */
				temp_scale=frame[11]|(frame[12]<<8);
				temp_scale=(temp_scale*9)/16;
				frame[11]=(temp_scale&0xff);
				frame[12]=(temp_scale>>8)&0xf;


				ym_play_frame(frame,shift_size,
					&ds, diff_mode,play_music,mute_channel);

				if (visualize) {
					if (display_type&DISPLAY_TEXT) {
						printf("\033[H\033[2J");
					}

					display_update(display_type,
							&ds,
							frame_num,
							100,
							filename,0,
							current_mode);
				}

				result=handle_keypress();
				if (result) return result;

				frames_elapsed++;
				frame_num++;

				sprintf(string,"%s %s %s",
					pt3_current_note('A',&pt3),
					pt3_current_note('B',&pt3),
					pt3_current_note('C',&pt3));

				music_visualize(frames_elapsed,frame_num,
					filename,
					length_seconds,pt3.name,pt3.author,
					string);

				/* Calculate time it took to play/visualize */
				gettimeofday(&next,NULL);
				s=start.tv_sec+(start.tv_usec/1000000.0);
				n=next.tv_sec+(next.tv_usec/1000000.0);
				diff=(n-s)*1000000.0;

				/* Delay until time for next update (often 50Hz) */
				if (play_music) {
					if (diff>0) bcm2835_delayMicroseconds(20000-diff);
					/* often 50Hz = 20000 */
					/* TODO: calculate correctly */

				}
				else {
					if (visualize) usleep(1000000/frame_rate);
				}

				/* Calculate time it actually took, and print		*/
				/* so we can see if things are going horribly wrong	*/
				gettimeofday(&next,NULL);
				n=next.tv_sec+(next.tv_usec/1000000.0);

				if (frame_num%100==0) {
					hz=1/(n-s);
					printf("Done frame %d/%d, %.1lfHz\n",
						frame_num,1,hz);
				}
				start.tv_sec=next.tv_sec;
				start.tv_usec=next.tv_usec;
			}
		}
	}

	return 0;
}


int main(int argc, char **argv) {

	char filename[BUFSIZ]="intro2.ym";
	int result;
	int c;
	int next_song,first_song;

	/* Setup control-C handler to quiet the music	*/
	/* otherwise if you force quit it keeps playing	*/
	/* the last tones */
	signal(SIGINT, quiet_and_exit);

	/* Set to have highest possible priority */
	display_enable_realtime();

	/* Parse command line arguments */
	while ((c = getopt(argc, argv, "dDmhvmsnitr"))!=-1) {
		switch (c) {
			case 'd':
				/* Debug messages */
				printf("Debug enabled\n");
				dump_info=1;
				break;
			case 'D':
				/* diff mode */
				printf("Diff mode\n");
				diff_mode=1;
				break;
			case 'h':
				/* help */
				print_help(0,argv[0]);
				break;
			case 'v':
				/* version */
				print_help(1,argv[0]);
				break;
			case 'm':
				/* mono sound */
				shift_size=8;
				break;
			case 's':
				/* stereo sound */
				shift_size=16;
				break;
			case 'n':
				/* no sound */
				play_music=0;
				break;
			case 'i':
				/* i2c visualization */
				display_type=DISPLAY_I2C;
				break;
			case 't':
				/* text visualization */
				display_type=DISPLAY_TEXT;
				break;
			case 'r':
				/* repeat */
				music_repeat=1;
				break;
			default:
				print_help(0,argv[0]);
				break;
		}
	}

	first_song=optind;
	next_song=0;

	/* Initialize the Chip interface */
	if (play_music) {
		result=initialize_ay_3_8910(0);
		if (result<0) {
			printf("Error initializing bcm2835!\n");
			printf("Maybe try running as root?\n\n");
			exit(0);
		}
		result=max98306_init();
		if (result<0) {
			printf("Error initializing max98306 amp\n");
			exit(0);
		}

		printf("Headphone is: %d\n",
			max98306_check_headphone());

		if (amp_disable) {
			result=max98306_disable();
		}
		else {
			result=max98306_enable();
		}

	}

	/* Initialize the displays */
	if (visualize) {
		result=display_init(display_type);
		if (result<0) {
			printf("Error initializing display!\n");
			printf("Turning off display for now!\n");
			display_type=0;
		}
	}

	while(1) {

		if (argv[first_song+next_song]!=NULL) {
			strcpy(filename,argv[first_song+next_song]);
			next_song++;
		}
		else {
			break;
		}

		/* Play the song */
		result=play_song(filename);

		/* Handle actions */

		if (result==CMD_EXIT_PROGRAM) {
			break;
		}

		if (result==CMD_BACK) {
			next_song-=2;
			if (next_song<0) next_song=0;
		}
		if (result==CMD_NEXT) {
			/* already point to next song */
		}

		/* Quiet down the chips */
		if (play_music) {
			quiet_ay_3_8910(shift_size);
		}

		usleep(500000);

	}

	/* Get ready to shut down */

	/* Quiet down the chips */
	if (play_music) {
		quiet_ay_3_8910(shift_size);
		close_ay_3_8910();
		max98306_free();
	}

	/* Clear out display */
	if (visualize) {
		display_shutdown(display_type);
	}

	return 0;
}
