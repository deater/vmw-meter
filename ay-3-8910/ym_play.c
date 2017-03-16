/* Parse/Play a YM AY-3-8910 Music File */
/* Used file info found here: http://leonard.oxg.free.fr/ymformat.html */
/* Also useful: ftp://ftp.modland.com/pub/documents/format_documentation/Atari%20ST%20Sound%20Chip%20Emulator%20YM1-6%20(.ay,%20.ym).txt */

#define VERSION "0.5"

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

#include "ay-3-8910.h"
#include "display.h"
#include "ym_lib.h"
#include "max98306.h"

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
static int headphones=1;

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
	printf("\tfilename: must be uncompressed YM5 file for now\n\n");

	exit(0);
}

#define TEXT_MODE_BANNER	0
#define TEXT_MODE_FILENAME	1
#define TEXT_MODE_SONGNAME	2
#define TEXT_MODE_AUTHOR	3
#define TEXT_MODE_TIMER		4
#define TEXT_MODE_MENU		5

static int play_song(char *filename) {

	int length_seconds;
	double s,n,hz,diff;

	int display_command=0,result;
	int frame_num=0,frames_elapsed=0;

	struct timeval start,next;

	struct ym_song_t ym_song;
	struct display_stats ds;
	struct frame_stats fs;

	int text_mode=TEXT_MODE_BANNER;
	char display_text[13];

	printf("\nPlaying song %s\n",filename);

	result=load_ym_song(filename,&ym_song);
	if (result<0) {
		return -1;
	}

	gettimeofday(&start,NULL);

	/**********************/
	/* Print song summary */
	/**********************/

	printf("\tYM%d",ym_song.type);
	printf("\tSong attributes (%d) : ",ym_song.attributes);
	printf("Interleaved=%s\n",ym_song.interleaved?"yes":"no");
	if (ym_song.num_digidrum>0) {
		printf("Num digidrum samples: %d\n",ym_song.num_digidrum);
	}
	printf("\tFrames: %d, ",ym_song.num_frames);
	printf("Chip clock: %d Hz, ",ym_song.master_clock);
	printf("Frame rate: %d Hz, ",ym_song.frame_rate);
	if (ym_song.frame_rate!=50) {
		fprintf(stderr,"FIX ME framerate %d\n",ym_song.frame_rate);
		exit(1);
	}
	length_seconds=ym_song.num_frames/ym_song.frame_rate;
	printf("Length=%d:%02d\n",length_seconds/60,length_seconds%60);
	printf("\tLoop frame: %d, ",ym_song.loop_frame);
	printf("Extra data size: %d\n",ym_song.extra_data);
	printf("\tSong name: %s\n",ym_song.song_name);
	printf("\tAuthor name: %s\n",ym_song.author);
	printf("\tComment: %s\n",ym_song.comment);

	sprintf(display_text,"VMW CHIPTUNE");
	display_string(display_type,display_text);

	/******************/
	/* Play the song! */
	/******************/

	frame_num=0;
	while(1) {

		if (!music_paused) {

			ym_play_frame(&ym_song,frame_num,shift_size,
					&fs,diff_mode,
					play_music,
					mute_channel);

			if (visualize) {
				if (display_type&DISPLAY_TEXT) {
					printf("\033[H\033[2J");
				}

				ds.a_bar=fs.a_bar;
				ds.b_bar=fs.b_bar;
				ds.c_bar=fs.c_bar;

				ds.a_freq=fs.a_freq;
				ds.b_freq=fs.b_freq;
				ds.c_freq=fs.c_freq;

				display_update(display_type,
							&ds,
							frame_num,
							ym_song.num_frames,
							filename,0);
			}
		}

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
			if (visualize) usleep(1000000/ym_song.frame_rate);
		}

		/* Calculate time it actually took, and print		*/
		/* so we can see if things are going horribly wrong	*/
		gettimeofday(&next,NULL);
		s=start.tv_sec+(start.tv_usec/1000000.0);
		n=next.tv_sec+(next.tv_usec/1000000.0);

		if (frame_num%100==0) {
			hz=1/(n-s);
			printf("Done frame %d/%d, %.1lfHz\n",
				frame_num,ym_song.num_frames,hz);
		}
		start.tv_sec=next.tv_sec;
		start.tv_usec=next.tv_usec;

		/* Handle keypresses */
		do {
			display_command=display_read_keypad(display_type);

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
			if (display_command==CMD_MUTE_N) {
				if (mute_channel&0x05) mute_channel&=~0x05;
				else mute_channel|=0x05;
				printf("NEW %x\n",mute_channel);
			}

			if (display_command==CMD_EXIT_PROGRAM) {
				free(ym_song.file_data);
				return CMD_EXIT_PROGRAM;
			}

			/* prev song */
			if (display_command==CMD_BACK) {
				free(ym_song.file_data);
				return CMD_BACK;
			}
			/* next song */
			if (display_command==CMD_NEXT) {
				free(ym_song.file_data);
				return CMD_NEXT;
			}

			/* rewind = Beginning of track */
			if (display_command==CMD_RW) {
				frame_num=0;
			}

			/* fastfwd = skip ahead 5s */
			if (display_command==CMD_FF) {
				frame_num+=5*ym_song.frame_rate;
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

			if (display_command==CMD_HEADPHONE_IN) {
				max98306_disable();
			}

			if (display_command==CMD_HEADPHONE_OUT) {
				max98306_enable();
			}



			if (display_command==CMD_LOOP) {
				music_loop=!music_loop;
				if (music_loop) printf("MUSIC LOOP ON\n");
				else printf("MUSIC LOOP OFF\n");
			}
			/* Avoid spinning CPU if paused */
			if (music_paused) usleep(100000);
		} while (music_paused);

		/* increment frame */
		if (!music_paused) frame_num++;

		/* Check to see if done with file */
		if (frame_num>=ym_song.num_frames) {
			if (music_loop) {
				frame_num=ym_song.loop_frame;
			}
			else {
				break;
			}
		}

		frames_elapsed++;

		if (frames_elapsed==50) {
			text_mode=TEXT_MODE_FILENAME;
			memset(display_text,0,13);
			snprintf(display_text,13,filename);
			display_string(display_type,display_text);
		}

		if (frames_elapsed==150) {
			text_mode=TEXT_MODE_SONGNAME;
			memset(display_text,0,13);
			snprintf(display_text,13,ym_song.song_name);
			display_string(display_type,display_text);
		}

		if (frames_elapsed==250) {
			text_mode=TEXT_MODE_AUTHOR;
			memset(display_text,0,13);
			snprintf(display_text,13,ym_song.author);
			display_string(display_type,display_text);
		}
		if (frames_elapsed>350) {
			if (frames_elapsed%25==0) {
				text_mode=TEXT_MODE_TIMER;
				memset(display_text,0,12);
				snprintf(display_text,13,"%2d/%02d--%2d/%02d",
					(frame_num/50)/60,(frame_num/50)%60,
					length_seconds/60,length_seconds%60);
				display_string(display_type,display_text);
			}
		}
	}

	if (frame_num>ym_song.num_frames) {
		printf("Fast-forwarded, skipping end check\n");
	}
	else {
		int file_offset;

		file_offset=(frame_num)*ym_song.frame_size;

		if (ym_song.type>3) {
			/* Read the tail of the file and ensure */
			/* it has the proper trailer */
			if (memcmp(&ym_song.frame_data[file_offset],"End!",4)) {
				fprintf(stderr,"ERROR! Bad ending! %x\n",
					ym_song.frame_data[file_offset]);
				return -1;
			}
		}

	}

	/* Free the ym file */
	free(ym_song.file_data);

//	if (dump_info) {
//		printf("Max a=%.2lf b=%.2lf c=%.2lf\n",max_a,max_b,max_c);
//	}

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
	setpriority(PRIO_PROCESS, 0, -20);

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
		result=initialize_ay_3_8910(1);
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
		result=max98306_enable();

		printf("Headphone is: %d\n",
			max98306_check_headphone());

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
