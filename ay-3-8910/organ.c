/* Musical Organ for ay-3-8910 */

#define VERSION "0.3"

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

#define AY38910_CLOCK	1000000	/* 1MHz on our board */

static int play_music=1;
static int dump_info=0;
static int shift_size=16;

static void quiet_and_exit(int sig) {

	if (play_music) {
		quiet_ay_3_8910(shift_size);
		close_ay_3_8910();
	}

	printf("Quieting and exiting\n");
	_exit(0);

}

void print_help(int just_version, char *exec_name) {

	printf("\norgan version %s by Vince Weaver <vince@deater.net>\n\n",VERSION);
	if (just_version) exit(0);

	printf("This plays YM5 music files out of the VMW AY-3-8910 device\n");
	printf("on a suitably configured Raspberry Pi machine.\n\n");

	exit(0);
}

static int play_organ(void) {

	unsigned char frame[16];
	int frame_rate=50;
	int i,j;
	double s,n,hz,diff;
	double max_a=0.0,max_b=0.0,max_c=0.0;


	int a_period,b_period,c_period,n_period,e_period;
	double a_freq=0.0, b_freq=0.0, c_freq=0.0,n_freq=0.0,e_freq=0.0;
	int new_a,new_b,new_c,new_n,new_e;

	struct timeval start,next;

	int master_clock=1000000;	/* 1MHz */
	int num_frames=0;

	/**********************/
	/* Print song summary */
	/**********************/

	i=0;
	while(1) {

		/* get time */
		gettimeofday(&start,NULL);


		/****************************************/
		/* Write out the music			*/
		/****************************************/

		a_period=((frame[1]&0xf)<<8)|frame[0];
		b_period=((frame[3]&0xf)<<8)|frame[2];
		c_period=((frame[5]&0xf)<<8)|frame[4];
		n_period=frame[6]&0x1f;
		e_period=((frame[12]&0xff)<<8)|frame[11];

		if (a_period>0) a_freq=master_clock/(16.0*(double)a_period);
		if (b_period>0) b_freq=master_clock/(16.0*(double)b_period);
		if (c_period>0) c_freq=master_clock/(16.0*(double)c_period);
		if (n_period>0) n_freq=master_clock/(16.0*(double)n_period);
		if (e_period>0) e_freq=master_clock/(256.0*(double)e_period);

		if (dump_info) {
			printf("%05d:\tA:%04x B:%04x C:%04x N:%02x M:%02x ",
				i,a_period,b_period,c_period,n_period,frame[7]);

			printf("AA:%02x AB:%02x AC:%02x E:%04x,%02x %04x\n",
				frame[8],frame[9],frame[10],
				(frame[12]<<8)+frame[11],frame[13],
				(frame[14]<<8)+frame[15]);

			printf("\t%.1lf %.1lf %.1lf %.1lf %.1lf ",
				a_freq,b_freq,c_freq,n_freq, e_freq);
			printf("N:%c%c%c T:%c%c%c ",
				(frame[7]&0x20)?' ':'C',
				(frame[7]&0x10)?' ':'B',
				(frame[7]&0x08)?' ':'A',
				(frame[7]&0x04)?' ':'C',
				(frame[7]&0x02)?' ':'B',
				(frame[7]&0x01)?' ':'A');

			if (frame[8]&0x10) printf("VA: E ");
			else printf("VA: %d ",frame[8]&0xf);
			if (frame[9]&0x10) printf("VB: E ");
			else printf("VB: %d ",frame[9]&0xf);
			if (frame[10]&0x10) printf("VC: E ");
			else printf("VC: %d ",frame[10]&0xf);

			if (frame[13]==0xff) {
				printf("NOWRITE");
			}
			else {
				if (frame[13]&0x1) printf("Hold");
				if (frame[13]&0x2) printf("Alternate");
				if (frame[13]&0x4) printf("Attack");
				if (frame[13]&0x8) printf("Continue");
			}
			printf("\n");

			if (a_freq>max_a) max_a=a_freq;
			if (b_freq>max_b) max_b=b_freq;
			if (c_freq>max_c) max_c=c_freq;
		}


		frame[0]=new_a&0xff;	frame[1]=(new_a>>8)&0xf;
		frame[2]=new_b&0xff;	frame[3]=(new_b>>8)&0xf;
		frame[4]=new_c&0xff;	frame[5]=(new_c>>8)&0xf;
		frame[6]=new_n&0x1f;
		frame[11]=new_e&0xff;	frame[12]=(new_e>>8)&0xff;

		if (dump_info) {
			printf("\t%04x %04x %04x %04x %04x\n",
				new_a,new_b,new_c,new_n,new_e);

		}

		if (play_music) {
			for(j=0;j<13;j++) {
				write_ay_3_8910(j,frame[j],shift_size);
			}

			/* Special case.  Writing r13 resets it,	*/
			/* so special 0xff marker means do not write	*/
			if (frame[13]!=0xff) {
				write_ay_3_8910(13,frame[13],shift_size);
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
			usleep(20000);
		}

		/* Calculate time it actually took, and print		*/
		/* so we can see if things are going horribly wrong	*/
		gettimeofday(&next,NULL);
		s=start.tv_sec+(start.tv_usec/1000000.0);
		n=next.tv_sec+(next.tv_usec/1000000.0);

		if (i%100==0) {
			hz=1/(n-s);
			printf("Done frame %d/%d, %.1lfHz\n",i,num_frames,hz);
		}
		start.tv_sec=next.tv_sec;
		start.tv_usec=next.tv_usec;

	}

	return 0;
}


int main(int argc, char **argv) {

	int result;
	int c;

	/* Setup control-C handler to quiet the music	*/
	/* otherwise if you force quit it keeps playing	*/
	/* the last tones */
	signal(SIGINT, quiet_and_exit);

	/* Set to have highest possible priority */
	setpriority(PRIO_PROCESS, 0, -20);

	/* Parse command line arguments */
	while ((c = getopt(argc, argv, "dmhvmsnitr"))!=-1) {
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
			default:
				print_help(0,argv[0]);
				break;
		}
	}


	/* Initialize the Chip interface */
	if (play_music) {
		result=initialize_ay_3_8910(1);
		if (result<0) {
			printf("Error initializing bcm2835!\n");
			printf("Maybe try running as root?\n\n");
			exit(0);
		}
	}


	/* Play the song */
	result=play_organ();

	/* Quiet down the chips */
	if (play_music) {
		quiet_ay_3_8910(shift_size);
		close_ay_3_8910();
	}

	return 0;
}
