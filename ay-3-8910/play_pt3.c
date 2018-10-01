/* Parse/Play a PT3 pro-tracker 3 zx file */

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

#include "stats.h"
#include "ay-3-8910.h"
#include "display.h"
#include "ym_lib.h"
#include "max98306.h"
#include "visualizations.h"

#include "version.h"

//#define AY38910_CLOCK	1000000	/* 1MHz on our board */

static int play_music=1;
static int mute_channel=0;
static int dump_info=0;
static int shift_size=16;
static int music_repeat=0;
static int music_paused=0;
static int music_loop=0;
static int volume=1;

static void quiet_and_exit(int sig) {

	if (play_music) {
		quiet_ay_3_8910(shift_size);
		close_ay_3_8910();
		max98306_free();
	}

	printf("Quieting and exiting\n");
	_exit(0);

}

static void print_help(int just_version, char *exec_name) {

	printf("\nplay_pt3 version %s by Vince Weaver <vince@deater.net>\n\n",VERSION);
	if (just_version) exit(0);

	printf("This plays PT3 music files out of the VMW AY-3-8910 device\n");
	printf("on a suitably configured Raspberry Pi machine.\n\n");

	printf("Usage:\n");
	printf("\t%s [-h] [-v] [-d] [-i] [-t] [-m] [-s] [-n] filename\n\n",
		exec_name);
	printf("\t-h: this help message\n");
	printf("\t-v: version info\n");
	printf("\t-d: print debug messages\n");

	exit(0);
}

#define TEXT_MODE_BANNER	0
#define TEXT_MODE_FILENAME	1
#define TEXT_MODE_SONGNAME	2
#define TEXT_MODE_AUTHOR	3
#define TEXT_MODE_TIMER		4
#define TEXT_MODE_MENU		5


/* TIMING */
/* for 50Hz have a 20ms deadline */
/* Currently (with GPIO music and Linux i2c):			   */
/*   ym_play_frame:        14ms (need to move to SPI!)             */
/*   display_update:        1-3ms (depends on if all are updated)  */
/*   display_keypad_read: 0.5ms                                    */
/*   display_string:        5ms (slow!  3 i2c addresses            */
/*				5.02ms (Linux) 4.9ms (libbcm)	   */

static int play_song(char *filename) {

	int length_seconds;
	double s,n,hz,diff;
	int result;
	int frame_num=0,frames_elapsed=0;

	struct timeval start,next;

	struct ym_song_t ym_song;
	struct display_stats ds;

#define TIMING_DEBUG	0

#if TIMING_DEBUG==1
	struct timeval before,after;
	double b,a;
	FILE *debug_file;
	debug_file=fopen("timing.debug","w");
#endif

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

	/******************/
	/* Play the song! */
	/******************/

	frame_num=0;
	while(1) {

		if (!music_paused) {

#if TIMING_DEBUG==1
		gettimeofday(&before,NULL);
#endif

			ym_play_frame(&ym_song,frame_num,shift_size,
					&ds,0,
					play_music,
					mute_channel);


#if TIMING_DEBUG==1
		gettimeofday(&after,NULL);
		b=before.tv_sec+(before.tv_usec/1000000.0);
		a=after.tv_sec+(after.tv_usec/1000000.0);
		fprintf(debug_file,"T: %lf\n",(a-b));
#endif

		}

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
#if TIMING_DEBUG==1
			fprintf(debug_file,"D: %lf\n",(20000-diff));
#endif
		}

		/* Calculate time it actually took, and print		*/
		/* so we can see if things are going horribly wrong	*/
		gettimeofday(&next,NULL);
		n=next.tv_sec+(next.tv_usec/1000000.0);

		if (frame_num%100==0) {
			hz=1/(n-s);
			printf("Done frame %d/%d, %.1lfHz\n",
				frame_num,ym_song.num_frames,hz);
		}
		start.tv_sec=next.tv_sec;
		start.tv_usec=next.tv_usec;



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

#if TIMING_DEBUG==1
	fclose(debug_file);
#endif

//	if (dump_info) {
//		printf("Max a=%.2lf b=%.2lf c=%.2lf\n",max_a,max_b,max_c);
//	}

	return 0;
}


int main(int argc, char **argv) {

	char filename[BUFSIZ]="ea.pt3";
	int result;
	int c,first_song=0,next_song;

	/* Setup control-C handler to quiet the music	*/
	/* otherwise if you force quit it keeps playing	*/
	/* the last tones */
	signal(SIGINT, quiet_and_exit);

	/* Parse command line arguments */
	while ((c = getopt(argc, argv, "dDmhvmsnitr"))!=-1) {
		switch (c) {
			case 'd':
				/* Debug messages */
				printf("Debug enabled\n");
				dump_info=1;
				break;
			case 'h':
				/* help */
				print_help(0,argv[0]);
				break;
			case 'v':
				/* version */
				print_help(1,argv[0]);
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

	return 0;
}
