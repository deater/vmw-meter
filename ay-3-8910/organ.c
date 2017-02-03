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
#include <termios.h>
#include <sys/resource.h>

#include <bcm2835.h>

#include "ay-3-8910.h"

#include "notes.h"

#define AY38910_CLOCK	1000000	/* 1MHz on our board */

static int play_music=1;
static int dump_info=0;
static int shift_size=16;

static int current_instrument=0;

#define MAX_INSTRUMENTS 6

struct instrument_type {
	int envelope[10];
	int length;
	char *name;
} instruments[MAX_INSTRUMENTS] = {
	{
	.name="raw",
	.envelope={15,15,15,15,15,15,15,15,15,15},
	.length=10,
	},
	{
	.name="piano",
	.envelope={14,15,15,15,15,15,13,11,9,7},
	.length=10,
	},
	{
	.name="triangle",
	.envelope={7,9,11,13,15,15,13,11,9,7},
	.length=10,
	},
	{
	.name="trill",
	.envelope={9,15,9,15,9,15,9,15,9,15},
	.length=10,
	},
	{
	.name="sine",
	.envelope={8,10,15,15,10,8,6,1,1,6},
	.length=10,
	},
	{
	.name="long trill",
	.envelope={9,15,15,15,9,9,15,15,9,9},
	.length=10,
	},

};


static struct termios saved_tty;

static void quiet_and_exit(int sig) {

	if (play_music) {
		quiet_ay_3_8910(shift_size);
		close_ay_3_8910();
	}

	tcsetattr (0, TCSANOW, &saved_tty);

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
	int i,j;
	double s,n,hz,diff;

	double freq;

	struct timeval start,next;

	int master_clock=1000000;	/* 1MHz */

	int frames=0;

	int result,ch;
	int quit=0;

	int octave=4;
	int a_freq=0,a_length=0,a_enabled=0;

	struct termios new_tty;

	/* Save currenty term settings */
	tcgetattr (0, &saved_tty);
	tcgetattr (0, &new_tty);

	/* Put term in raw keryboard mode */
	new_tty.c_lflag &= ~ICANON;
	new_tty.c_cc[VMIN] = 1;
	new_tty.c_lflag &= ~ECHO;
	tcsetattr (0, TCSANOW, &new_tty);
	/* Make it nonblocking too */
	fcntl (0, F_SETFL, fcntl (0, F_GETFL) | O_NONBLOCK);

	for(i=0;i<15;i++) frame[i]=0;

	i=0;
	while(1) {

		/* get time */
		gettimeofday(&start,NULL);


		/* Read from Keyboard */
		result=read(0,&ch,1);
		if (result<0) {
		}
		else {
			switch (ch) {

			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
				current_instrument=ch-'0';
				printf("New instrument: %s\n",
					instruments[current_instrument].name);
				break;

			case 'q': quit=1; break;

			case 'a':
				printf("a");
				freq=note_to_freq('C',0,0,octave);
				a_freq=master_clock/(16.0*freq);
				a_enabled=1;
				a_length=instruments[current_instrument].length;
				break;
			case 's':
				freq=note_to_freq('D',0,0,octave);
				a_freq=master_clock/(16.0*freq);
				a_enabled=1;
				a_length=instruments[current_instrument].length;
				break;
			case 'd':
				freq=note_to_freq('E',0,0,octave);
				a_freq=master_clock/(16.0*freq);
				a_enabled=1;
				a_length=instruments[current_instrument].length;
				break;
			case 'f':
				freq=note_to_freq('F',0,0,octave);
				a_freq=master_clock/(16.0*freq);
				a_enabled=1;
				a_length=instruments[current_instrument].length;
				break;
			case 'g':
				freq=note_to_freq('G',0,0,octave);
				a_freq=master_clock/(16.0*freq);
				a_enabled=1;
				a_length=instruments[current_instrument].length;
				break;
			case 'h':
				freq=note_to_freq('A',0,0,octave);
				a_freq=master_clock/(16.0*freq);
				a_enabled=1;
				a_length=instruments[current_instrument].length;
				break;
			case 'j':
				freq=note_to_freq('B',0,0,octave);
				a_freq=master_clock/(16.0*freq);
				a_enabled=1;
				a_length=instruments[current_instrument].length;
				break;

			}
		}
		if (quit) break;

		/****************************************/
		/* Write out the music			*/
		/****************************************/

		if (a_enabled) {
			frame[0]=a_freq&0xff;
			frame[1]=(a_freq>>8)&0xf;
			frame[7]=0x38;
			//printf("Before: %x %d %d\n",
			//	frame[8],10-a_length,envelope[10-a_length]);
			frame[8]=instruments[current_instrument].envelope[10-a_length]; //envelope[a_length-10];  // amp A
		}
		else {
			frame[8]=0x0;
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

		if (frames%250==0) {
			hz=1/(n-s);
			printf("Done frame %d, %.1lfHz\n",frames,hz);
		}
		start.tv_sec=next.tv_sec;
		start.tv_usec=next.tv_usec;

		frames++;
		if (a_length) a_length--;
		if (a_length==0) a_enabled=0;
	}

	tcsetattr (0, TCSANOW, &saved_tty);

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
