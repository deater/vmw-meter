/* Musical Organ for ay-3-8910 */

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
#include "max98306.h"

#include "display.h"

#include "notes.h"

#include "version.h"

static int display_type=DISPLAY_I2C;

#define AY38910_CLOCK	1000000	/* 1MHz on our board */

static int play_music=1;
static int dump_info=0;
static int shift_size=16;

static int current_instrument=0;

#define MAX_DRUMS	10
#define MAX_INSTRUMENTS 6
#define MAX_SOUNDS	2

struct instrument_type {
	int envelope[128];
	int length;
	char *name;
};

struct drum_type {
	int envelope[32];
	int period[32];
	int period2[32];
	int length;
	char *name;
};

struct sound_type {
	int envelope[40];
	int period[40];
	int length;
	char *name;
};

struct sound_type sound[MAX_SOUNDS] = {
	{
	.name="monkey1",
	.period   = {851,213,169,142,106,142,169,213,169,213,
			169,142,169,142,169,213,169,142,142,},
	.envelope = {15 , 15, 13, 13, 12, 12, 11, 11, 10, 10,
			13,  13, 12, 12, 11, 11, 10,  9, 9,},
	.length=18,
	},
	{
	.name="monkey2",
	.period=   {758,379,758,379,379,758,758,379,379,758,758,379,758,},
	.envelope= {  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9},
	.length=13,
	},
};



struct drum_type drum[MAX_DRUMS] = {
	{
	.name="raw",// 0
	.envelope={15,15,15,15,15,15,15,15,15,15},
	.period=  { 5, 5, 5, 5, 5, 5, 5, 5, 5, 5},
	.period2= { 668,668,668,668,668},
	.length=5,
	},
	{
	.name="bass",// 1
//	.envelope={15,15,15, 9, 9, 9, },
//	.period=  { 8, 8, 8, 8, 8, 8 },
//	.envelope={13,13,11, 11, 8, 8, },
//	.period=  { 10, 10, 10, 10, 10, 10 },
	.envelope={13,13,13, 0,0,0, },
	.period=  { 30, 20, 20, 0, 0, 0 },
	.period2= { 668,668,668,668,668,668},
	.length=3,
	},
	{
	.name="snare",// 2
	.envelope={14,15,15,13, 12, 9 },
	.period=  { 4, 5, 6, 7, 8, 8 },
	.period2= { 668,668,668,668,668},
	.length=6,
	},
	{
	.name="cymbal",// 3
	.envelope={12,12,10,10, 9, 9},
	.period=  { 1, 1, 1, 1, 1, 0},
	.period2= { 668,668,668,668,668,668},
	.length=6,
	},
	{
	.name="snare2",// 4    FRAME 2196 CPCTL10A
	.envelope={13, 11, 9, 7, 5, 3, 12, 10, 8, 6, 4, 2, 14},
	.period=  { 1,  1, 1, 1, 1, 1,  1,  1, 1, 1, 1, 1,  1},
	.period2= { 0,  0, 0, 0, 0, 0,  0,  0, 0, 0, 0, 0,  0},
	.length=13,
	},
	{
	.name="snare3",// 5    FRAME 2220 CPCTL10A
	.envelope={12, 10, 8, 6, 4, 2,  9,  7, 5, 3, 1, 0, 14},
	.period=  { 1,  1, 1, 1, 1, 1,  1,  1, 1, 1, 1, 1,  6},
	.period2= { 0,  0, 0, 0, 0, 0,  0,  0, 0, 0, 0, 0, 0x190},
	.length=13,
	},
	{
	.name="snare4",// 6    FRAME 2220 CPCTL10A
	.envelope={ 0, 14},
	.period=  { 1,  6},
	.period2= { 0, 0x190},
	.length=2,
	},
};

struct instrument_type instruments[MAX_INSTRUMENTS] = {
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
//	{
//	.name="triangle",
//	.envelope={7,9,11,13,15,15,13,11,9,7},
//	.length=10,
//	},
	{
	.name="triangle",
	.envelope={13,14,15,14,13,12,9,12},
	.length=8,
	},
	{
	.name="trill",
	.envelope={9,15,9,15,9,15,9,15,9,15},
	.length=10,
	},
//	{
//	.name="sine",
//	.envelope={8,10,15,15,10,8,6,1,1,6},
//	.length=10,
//	},
//	{
//	.name="sine",
//	.envelope={8,10,15,15,10,8,6,1,1,6},
//	.length=10,
//	},
	{
	.name="sine",
	.envelope={11,12,13,14,14,14,14,13,12,11},
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
		max98306_free();
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
	int a_period=0,a_length=0,a_enabled=0;
	int b_period=0,b_length=0,b_enabled=0;

	int n_count=0,n_enabled=0,n_type=0;
	int c_count=0,c_enabled=0,c_type=0;

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

		/* Read from keypad */
		ch=display_raw_keypad_read(display_type);

		if (ch==0) {
			/* Try to read from Keyboard */
			result=read(0,&ch,1);
			if (result<0) ch=0;
		}

		if (ch==0) {
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

			case CMD_RW:
			case 'a':
				freq=note_to_freq('C',0,0,octave,0);
				a_period=master_clock/(16.0*freq);
				a_enabled=1;
				a_length=instruments[current_instrument].length;
				break;
			case CMD_BACK:
			case 's':
				freq=note_to_freq('D',0,0,octave,0);
				a_period=master_clock/(16.0*freq);
				a_enabled=1;
				a_length=instruments[current_instrument].length;
				break;
			case CMD_MENU:
			case 'd':
				freq=note_to_freq('E',0,0,octave,0);
				a_period=master_clock/(16.0*freq);
				a_enabled=1;
				a_length=instruments[current_instrument].length;
				break;
			case CMD_PLAY:
			case 'f':
				freq=note_to_freq('F',0,0,octave,0);
				a_period=master_clock/(16.0*freq);
				a_enabled=1;
				a_length=instruments[current_instrument].length;
				break;
			case CMD_STOP:
			case 'g':
				freq=note_to_freq('G',0,0,octave,0);
				a_period=master_clock/(16.0*freq);
				a_enabled=1;
				a_length=instruments[current_instrument].length;
				break;
			case CMD_CANCEL:
			case 'h':
				freq=note_to_freq('A',0,0,octave,0);
				a_period=master_clock/(16.0*freq);
				a_enabled=1;
				a_length=instruments[current_instrument].length;
				break;
			case CMD_NEXT:
			case 'j':
				freq=note_to_freq('B',0,0,octave,0);
				a_period=master_clock/(16.0*freq);
				a_enabled=1;
				a_length=instruments[current_instrument].length;
				break;
			case CMD_FF:
			case 'k':
				freq=note_to_freq('C',0,0,octave+1,0);
				a_period=master_clock/(16.0*freq);
				a_enabled=1;
				a_length=instruments[current_instrument].length;
				break;

			case ',':
				n_type=1;
				n_enabled=1;
				n_count=0;

				b_enabled=1;
				b_length=drum[n_type].length;
				break;

			case '.':
				n_type=2;
				n_enabled=1;
				n_count=0;

				b_enabled=1;
				b_period=656;
				b_length=drum[n_type].length;
				break;

			case '/':
				n_type=3;
				n_enabled=1;
				n_count=0;

				b_enabled=1;
				b_period=0;
				b_length=drum[n_type].length;
				break;

			case 'p':
				c_type=0;
				c_enabled=1;
				c_count=0;
				break;

			case 'o':
				c_type=1;
				c_enabled=1;
				c_count=0;
				break;


			}
		}
		if (quit) break;

		/****************************************/
		/* Write out the music			*/
		/****************************************/

		for(i=0;i<15;i++) frame[i]=0;

		if (a_enabled) {
			frame[0]=a_period&0xff;
			frame[1]=(a_period>>8)&0xf;
		}


		if (b_enabled) {
			b_period=drum[n_type].period2[n_count];
			frame[2]=b_period&0xff;
			frame[3]=(b_period>>8)&0xf;
		}

		if (c_enabled) {
			frame[4]=(sound[c_type].period[c_count])&0xff;
			frame[5]=((sound[c_type].period[c_count])>>8)&0xf;
		}

		if (n_enabled) {
			frame[6]=drum[n_type].period[n_count];
		}

		/* Setup mixer */
		if (!c_enabled) frame[7]|=0x4;
		if (!b_enabled) frame[7]|=0x2;
		if (!a_enabled) frame[7]|=0x1;
		if (!n_enabled) frame[7]|=0x10;
		frame[7]|=0x20;
		frame[7]|=0x08;


		/* Setup amplitude */
		if (a_enabled) {
			frame[8]=instruments[current_instrument].envelope[10-a_length]; //envelope[a_length-10];  // amp A
		}

		if (n_enabled) {
			frame[9]=drum[n_type].envelope[n_count];
		}

		if (c_enabled) {
			frame[10]=sound[c_type].envelope[c_count];
		}



		if (play_music) {
			for(j=0;j<13;j++) {
				write_ay_3_8910(j,frame[j],frame[j],shift_size);
			}

			/* Special case.  Writing r13 resets it,	*/
			/* so special 0xff marker means do not write	*/
			if (frame[13]!=0xff) {
				write_ay_3_8910(13,frame[13],frame[13],shift_size);
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
		if (b_length) b_length--;

		if (c_enabled) c_count++;
		if (n_enabled) n_count++;

		if (a_length==0) a_enabled=0;
		if (b_length==0) b_enabled=0;

		if ((c_enabled) && (c_count>sound[c_type].length)) {
			c_enabled=0;
		}

		if ((n_enabled) && (n_count>drum[n_type].length)) {
//			printf("Disabling b and n, %d > %d\n",
//				n_count,drum[n_type].length);
			n_enabled=0;
			b_enabled=0;
		}

//		if (n_enabled || b_enabled) {
//			printf("%d %d %d\n",n_count,n_enabled,b_enabled);
//		}
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


	result=display_init(DISPLAY_I2C);
        if (result<0) {
                display_type=DISPLAY_TEXT;
        }


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
			printf("Error initializing max98306 amp!!\n");
			play_music=0;
		}
		else {
			result=max98306_enable();
		}
	}


	/* Play the song */
	result=play_organ();

	/* Quiet down the chips */
	if (play_music) {
		quiet_ay_3_8910(shift_size);
		close_ay_3_8910();
		max98306_free();
	}

	display_shutdown(display_type);

	return 0;
}
