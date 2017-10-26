/* Falling tetromino game */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>

#include <errno.h>

#include <sys/resource.h>
#include <sys/time.h>

#include <bcm2835.h>

//#include "i2c_lib.h"

#include "ymlib/stats.h"
#include "display.h"
#include "ay-3-8910.h"
#include "max98306.h"
#include "ym_lib.h"



static int play_music=1;
static int shift_size=16;
static int display_type=DISPLAY_I2C;


#define DISPLAY_SIZE	16
#define SPRITE_SIZE	4

#define NUM_PIECES	7
#define ROTATIONS	4

#define T_O	0
#define T_I	1
#define T_T	2
#define T_S	3
#define T_Z	4
#define T_J	5
#define T_L	6

struct piece_info_t {
	int x_left_edge;
	int x_right_edge;
	int y_top_edge;
	int y_bottom_edge;
} piece_info[NUM_PIECES][ROTATIONS] = {

	/* O */
	{
{.x_left_edge=1,.x_right_edge=1,.y_top_edge=1,.y_bottom_edge=1,},/* Start  */
{.x_left_edge=1,.x_right_edge=1,.y_top_edge=1,.y_bottom_edge=1,},/* 90 CW  */
{.x_left_edge=1,.x_right_edge=1,.y_top_edge=1,.y_bottom_edge=1,},/* 180 CW */
{.x_left_edge=1,.x_right_edge=1,.y_top_edge=1,.y_bottom_edge=1,},/* 270 CW */
	},
	/* I */
	{
{.x_left_edge=0,.x_right_edge=0,.y_top_edge=1,.y_bottom_edge=2,},
{.x_left_edge=2,.x_right_edge=1,.y_top_edge=0,.y_bottom_edge=0,},
{.x_left_edge=0,.x_right_edge=0,.y_top_edge=2,.y_bottom_edge=1,},
{.x_left_edge=1,.x_right_edge=2,.y_top_edge=0,.y_bottom_edge=0,},
	},
	/* T */
	{
{.x_left_edge=0,.x_right_edge=1,.y_top_edge=0,.y_bottom_edge=2,},
{.x_left_edge=1,.x_right_edge=1,.y_top_edge=0,.y_bottom_edge=1,},
{.x_left_edge=0,.x_right_edge=1,.y_top_edge=1,.y_bottom_edge=1,},
{.x_left_edge=0,.x_right_edge=2,.y_top_edge=0,.y_bottom_edge=1,},
	},
	/* S */
	{
{.x_left_edge=0,.x_right_edge=1,.y_top_edge=0,.y_bottom_edge=2,},
{.x_left_edge=1,.x_right_edge=1,.y_top_edge=0,.y_bottom_edge=1,},
{.x_left_edge=0,.x_right_edge=1,.y_top_edge=1,.y_bottom_edge=1,},
{.x_left_edge=0,.x_right_edge=2,.y_top_edge=0,.y_bottom_edge=1,},
	},
	/* Z */
	{
{.x_left_edge=0,.x_right_edge=1,.y_top_edge=0,.y_bottom_edge=2,},
{.x_left_edge=1,.x_right_edge=1,.y_top_edge=0,.y_bottom_edge=1,},
{.x_left_edge=0,.x_right_edge=1,.y_top_edge=1,.y_bottom_edge=1,},
{.x_left_edge=0,.x_right_edge=2,.y_top_edge=0,.y_bottom_edge=1,},
	},
	/* J */
	{
{.x_left_edge=0,.x_right_edge=1,.y_top_edge=0,.y_bottom_edge=2,},
{.x_left_edge=1,.x_right_edge=1,.y_top_edge=0,.y_bottom_edge=1,},
{.x_left_edge=0,.x_right_edge=1,.y_top_edge=1,.y_bottom_edge=1,},
{.x_left_edge=0,.x_right_edge=2,.y_top_edge=0,.y_bottom_edge=1,},
	},
	/* L */
	{
{.x_left_edge=0,.x_right_edge=1,.y_top_edge=0,.y_bottom_edge=2,},
{.x_left_edge=1,.x_right_edge=1,.y_top_edge=0,.y_bottom_edge=1,},
{.x_left_edge=0,.x_right_edge=1,.y_top_edge=1,.y_bottom_edge=1,},
{.x_left_edge=0,.x_right_edge=2,.y_top_edge=0,.y_bottom_edge=1,},
	},
};

int pieces[NUM_PIECES][ROTATIONS][SPRITE_SIZE][SPRITE_SIZE]= {
	{
	/* O */
	{{0,0,0,0},
	 {0,1,1,0},
	 {0,1,1,0},
	 {0,0,0,0}},
	{{0,0,0,0},
	 {0,1,1,0},
	 {0,1,1,0},
	 {0,0,0,0}},
	{{0,0,0,0},
	 {0,1,1,0},
	 {0,1,1,0},
	 {0,0,0,0}},
	{{0,0,0,0},
	 {0,1,1,0},
	 {0,1,1,0},
	 {0,0,0,0}},
	},

	{
	/* I */
	{{0,0,0,0},
	 {1,1,1,1},
	 {0,0,0,0},
	 {0,0,0,0}},
	{{0,0,1,0},
	 {0,0,1,0},
	 {0,0,1,0},
	 {0,0,1,0}},
	{{0,0,0,0},
	 {0,0,0,0},
	 {1,1,1,1},
	 {0,0,0,0}},
	{{0,1,0,0},
	 {0,1,0,0},
	 {0,1,0,0},
	 {0,1,0,0}},
	},

	{
	/* T */
	{{0,1,0,0},
	 {1,1,1,0},
	 {0,0,0,0},
	 {0,0,0,0}},
	{{0,1,0,0},
	 {0,1,1,0},
	 {0,1,0,0},
	 {0,0,0,0}},
	{{0,0,0,0},
	 {1,1,1,0},
	 {0,1,0,0},
	 {0,0,0,0}},
	{{0,1,0,0},
	 {1,1,0,0},
	 {0,1,0,0},
	 {0,0,0,0}},
	},

	{
	/* S */
	{{0,1,1,0},
	 {1,1,0,0},
	 {0,0,0,0},
	 {0,0,0,0}},
	{{0,1,0,0},
	 {0,1,1,0},
	 {0,0,1,0},
	 {0,0,0,0}},
	{{0,0,0,0},
	 {0,1,1,0},
	 {1,1,0,0},
	 {0,0,0,0}},
	{{1,0,0,0},
	 {1,1,0,0},
	 {0,1,0,0},
	 {0,0,0,0}},
	},

	{
	/* Z */
	{{1,1,0,0},
	 {0,1,1,0},
	 {0,0,0,0},
	 {0,0,0,0}},
	{{0,0,1,0},
	 {0,1,1,0},
	 {0,1,0,0},
	 {0,0,0,0}},
	{{0,0,0,0},
	 {1,1,0,0},
	 {0,1,1,0},
	 {0,0,0,0}},
	{{0,1,0,0},
	 {1,1,0,0},
	 {1,0,0,0},
	 {0,0,0,0}},
	},

	{
	/* J */
	{{1,0,0,0},
	 {1,1,1,0},
	 {0,0,0,0},
	 {0,0,0,0}},
	{{0,1,1,0},
	 {0,1,0,0},
	 {0,1,0,0},
	 {0,0,0,0}},
	{{0,0,0,0},
	 {1,1,1,0},
	 {0,0,1,0},
	 {0,0,0,0}},
	{{0,1,0,0},
	 {0,1,0,0},
	 {1,1,0,0},
	 {0,0,0,0}},
	},

	{
	/* L */
	{{0,0,1,0},
	 {1,1,1,0},
	 {0,0,0,0},
	 {0,0,0,0}},
	{{0,1,0,0},
	 {0,1,0,0},
	 {0,1,1,0},
	 {0,0,0,0}},
	{{0,0,0,0},
	 {1,1,1,0},
	 {1,0,0,0},
	 {0,0,0,0}},
	{{1,1,0,0},
	 {0,1,0,0},
	 {0,1,0,0},
	 {0,0,0,0}},
	},

};

/* http://tetris.wikia.com/wiki/Random_Generator */
/* Generates random bag with 7 pieces */
/* Empties bag before moving on */
static int Random_Generator(void) {

	/* FIXME */
	return rand()%7;
}

static void draw_piece(unsigned char *display_buffer, int which,
		int piece_x, int piece_y, int rotate) {

	int x,y;

	for(y=0;y<SPRITE_SIZE;y++) {
		for(x=0;x<SPRITE_SIZE;x++) {
			if (pieces[which][rotate][y][x]) {
				display_buffer[(y+piece_y)]|=1<<(7-(x+piece_x));
			}
		}
	}

}

static int bottom_collision(unsigned char *framebuffer,
		int which,int piece_x,int piece_y,int rotate) {

	int y_offset,x,y;

	y_offset=SPRITE_SIZE-
		piece_info[which][rotate].y_bottom_edge;

	/* check for floor */
	if (piece_y+y_offset > 15 ) return 1;

	/* Check for bottom collision */
	for(x=piece_info[which][rotate].x_left_edge;
			x<SPRITE_SIZE-piece_info[which][rotate].x_right_edge;
			x++) {
		for(y=y_offset-1;y>=0;y--) {
			if (pieces[which][rotate][y][x]) {
				if (framebuffer[piece_y+y+1]&
                                        1<<(7-(piece_x+x)))
//				if (display_8x16_vertical_getpixel(
//					framebuffer,piece_x+x,piece_y+y+1))
						return 1;
//				if (framebuffer[piece_y+y+1]&
//					1<<(7-(piece_x+x))) return 1;
				}
		}
	}

	/* No collision */
	return 0;
}



static int side_collision(unsigned char *framebuffer,
		int which,int piece_x,int piece_y,int rotate) {

	int x_left_offset;
	int x_right_offset;
	int x,y;

	x_left_offset=
		piece_info[which][rotate].x_left_edge;

	x_right_offset=SPRITE_SIZE-
		piece_info[which][rotate].x_right_edge;


	/* check for left wall */
	if (piece_x+x_left_offset<0) return 1;

	/* check for right wall */
	if (piece_x+x_right_offset>8) return 1;

	/* check if left edges are in a tower */
	for(y=piece_info[which][rotate].y_top_edge;
			y<SPRITE_SIZE-piece_info[which][rotate].y_bottom_edge;
			y++) {
		for(x=x_right_offset-1;x>=0;x--) {
			if (pieces[which][rotate][y][x])
//				if (display_8x16_vertical_getpixel(
//					framebuffer,piece_x+x,piece_y+y+1))
//						return 1;
				if (framebuffer[piece_y+y+1]&
					1<<(7-(piece_x+x))) return 1;
		}
	}

	/* check if right edges are in a tower */
	for(y=piece_info[which][rotate].y_top_edge;
			y<SPRITE_SIZE-piece_info[which][rotate].y_bottom_edge;
			y++) {
		for(x=x_left_offset;x<SPRITE_SIZE;x++) {
			if (pieces[which][rotate][y][x])
//				if (display_8x16_vertical_getpixel(
//					framebuffer,piece_x+x,piece_y+y+1))
//						return 1;
				if (framebuffer[piece_y+y+1]&
					1<<(7-(piece_x+x))) return 1;
		}
	}



	/* No collision */
	return 0;
}

static void update_aux_display(int next_piece,int lines) {

	char string[13];

	sprintf(string,"LINES%3d    ",lines);

	switch(next_piece) {
		case T_O: string[11]=0xd0; break;
		case T_I: string[11]=0xd1; break;
		case T_T: string[11]=0xd2; break;
		case T_J: string[11]=0xd3; break;
		case T_L: string[11]=0xd4; break;
		case T_S: string[11]=0xd5; break;
		case T_Z: string[11]=0xd6; break;
	}

	display_14seg_string(display_type,string);

}


static void update_our_display(int display_type, unsigned char *framebuffer) {

	unsigned char buffer[16];
	int i;

//		update_8x8_display_rotated(i2c_fd,
//			HT16K33_ADDRESS1,display_buffer,0,BROKEN);


	for(i=0;i<16;i++) buffer[i]=framebuffer[(15-i)];

	display_8x16_vertical(display_type,buffer);

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

struct effects_type {
	char name[128];
	int length;
	int envelope[32];
	int period[32];
	int noise;
	int noise_period[32];
} effects[4]={
	{
	.name="rotate",		// 0
	.length=6,
	.envelope={15,14,13},
	.period={477,425,379},
	.noise=0,
	},
	{
	.name="bottom",		// 1
	.length=3,
	.envelope={15,14,13},
	.period={29,29,29},
	.noise=0,
	},
	{
	.name="row",		// 2
	.length=13,
	.envelope={ 15,  15,  15,  15,  15,  15,  15,  15,  15,  15,  15,  15,  15},
	.period={758,379,758,379,379,758,758,379,379,758,758,379,758},
	.noise=1,
	.noise_period={10,9,8,7,6,5,4,3,2,1,0,0,0},
	},
	{			// 3
	.name="lose",
	.length=11,
	.envelope={15,14,15,14,15,14,15,14,15,14,13},
	.period={1702,1702,1911,1911,1911,1911,1911,1911,1911,1911,1911},
	.noise=0,
	},
};



int main(int arg, char **argv) {

	unsigned char framebuffer[DISPLAY_SIZE];
	unsigned char background[DISPLAY_SIZE];

	struct effects_type *a_effect=NULL,
			*b_effect=NULL,*c_effect=NULL;

	int a_effect_count=0,b_effect_count=0,c_effect_count=0;

	int piece_x=3, piece_y=0,piece_rotate=0,new_piece_x=0;
	int new_rotate;
	int piece_type,next_piece;
	int l,k;
	int score=0;
	int fractional_y=0;
	int level=1;
	int lines=0;
	int cleared_lines=0;

	int which_lines[4];

	int result;

	int i,ch;
	char text_string[13];
	int done=0;
	struct ym_song_t ym_song;
	int frame;
	int shift_size=16;

	struct timeval start,next;
        double s,n,diff;


	/* Setup control-C handler to quiet the music   */
	/* otherwise if you force quit it keeps playing */
	/* the last tones */
	signal(SIGINT, quiet_and_exit);

	/* Set to have highest possible priority */
	display_enable_realtime();

        /* Initialize the Chiptune interface */
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

	/* Init Displays */
	result=display_init(display_type);
	if (result<0) {
		printf("Error initializing display!\n");
		printf("Turning off display for now!\n");
		display_type=0;
	}


	/* Load music */
	result=load_ym_song("songs/korobeiniki.ym5",&ym_song);
        if (result<0) {
                return -1;
        }

start:

	display_keypad_clear(display_type);

	/* "Hit a Key" */
	sprintf(text_string,"%s","  HIT A KEY ");
	display_14seg_string(display_type,text_string);

	display_keypad_repeat_until_keypressed(display_type);

	/* Actually start */
	frame=0;
	gettimeofday(&start,NULL);

	/* Clear Framebuffer */
	for(i=0;i<DISPLAY_SIZE;i++) framebuffer[i]=0;

	piece_type=Random_Generator();
	next_piece=Random_Generator();

	update_aux_display(next_piece,lines);

	for(i=0;i<16;i++) {
		framebuffer[i]=0;
		background[i]=0;
	}

	unsigned char frame2[16];


	while(1) {
		/* Handle sound effects */
		memset(frame2,0,16);
		frame2[7]=0x38;

		if (a_effect) {

			frame2[0]=a_effect->period[a_effect_count]&0xff;
			frame2[1]=(a_effect->period[a_effect_count]>>8)&0xf;

			frame2[8]=a_effect->envelope[a_effect_count];

			if (a_effect->noise) {
				frame2[6]=a_effect->noise_period[a_effect_count];
				frame2[7]&=~0x08;
			}

			a_effect_count++;
			if (a_effect_count>a_effect->length) a_effect=NULL;
		}

		if (b_effect) {

			frame2[2]=b_effect->period[b_effect_count]&0xff;
			frame2[3]=(b_effect->period[b_effect_count]>>8)&0xf;

			frame2[9]=b_effect->envelope[b_effect_count];

			if (b_effect->noise) {
				frame2[6]=b_effect->noise_period[b_effect_count];
				frame2[7]&=~0x10;
			}

			b_effect_count++;
			if (b_effect_count>b_effect->length) b_effect=NULL;
		}

		if (c_effect) {
			frame2[4]=c_effect->period[c_effect_count]&0xff;
			frame2[5]=(c_effect->period[c_effect_count]>>8)&0xf;

			frame2[10]=c_effect->envelope[c_effect_count];

			if (c_effect->noise) {
				frame2[6]=c_effect->noise_period[c_effect_count];
				frame2[7]&=~0x20;
			}

			c_effect_count++;
			if (c_effect_count>c_effect->length) c_effect=NULL;
		}

//		ym_play_frame_effects(NULL,frame,shift_size,
		ym_play_frame_effects(&ym_song,frame,shift_size,
                                NULL, //stats
				play_music,
				frame2); // effects

		new_piece_x=piece_x;
		new_rotate=piece_rotate;

		/* Read Keyboard */
		ch=display_keypad_read(display_type);
		if (ch) {
			if (ch==CMD_BACK) {
				printf("Trying effect 0\n");
				a_effect=&effects[0];
				a_effect_count=0;
			}

			if (ch==CMD_EXIT_PROGRAM) {
				done=1;
				break;
			}
			if (ch==CMD_RW) { // '.'
				new_piece_x++;
			}
			if (ch==CMD_BACK) { // ','
				new_piece_x--;
			}
			if (ch==CMD_STOP) { // 's'
				piece_y++;
				score+=level;
			}
			if (ch==CMD_PLAY) { // ' '
				a_effect=&effects[0];
				a_effect_count=0;
				new_rotate--;
			}
			if (ch==CMD_MENU) { // 'm'
				a_effect=&effects[0];
				a_effect_count=0;
				new_rotate++;
			}
		}

		/* Handle rotate overflow */
		if (new_rotate>3) new_rotate=0;
		if (new_rotate<0) new_rotate=3;

		/* Check side collision */
		if (side_collision(background,piece_type,
					new_piece_x,piece_y,new_rotate)) {
			/* do not update */
		}
		else {
			piece_x=new_piece_x;
			piece_rotate=new_rotate;
		}

		/* copy background to framebuffer */
		for(i=0;i<16;i++) framebuffer[i]=background[i];

		/* Draw Piece on framebuffer*/
		draw_piece(framebuffer,piece_type,
			piece_x,piece_y,piece_rotate);


		update_our_display(display_type,framebuffer);

		// display 14-seg


		/* Gravity */
		fractional_y+=level;
		if (fractional_y>33) {
			fractional_y-=33;
			piece_y++;
		}

		/* check for bottom collision */
		if (bottom_collision(background,piece_type,piece_x,
					piece_y,piece_rotate)) {

			b_effect=&effects[1];
			b_effect_count=0;

			/* check if off top */
			if (piece_y==0) {
				b_effect=NULL;
				c_effect=NULL;

				a_effect=&effects[3];
				a_effect_count=0;

				printf("GAME OVER!\n");
				printf("Score=%d\n",score);
				memset(frame2,0,16);
				for(i=0;i<DISPLAY_SIZE;i++) {

		if (a_effect) {

			frame2[0]=a_effect->period[a_effect_count]&0xff;
			frame2[1]=(a_effect->period[a_effect_count]>>8)&0xf;

			frame2[8]=a_effect->envelope[a_effect_count];

			if (a_effect->noise) {
				frame2[6]=a_effect->noise_period[a_effect_count];
				frame2[7]&=~0x08;
			}

			a_effect_count++;
			if (a_effect_count>a_effect->length) a_effect=NULL;
		ym_play_frame_effects(NULL,frame,shift_size,
                                NULL, //stats
				play_music,
				frame2); // effects
		}

					framebuffer[i]=0xff;
					update_our_display(display_type,framebuffer);
					usleep(20000);
				}
				/* Display "Game" */
				sprintf(text_string,"%s"," GAME OVER  ");
				display_14seg_string(display_type,text_string);

				usleep(500000);

				level=1;
				lines=0;

				break;
			}

			/* place piece into background */

			draw_piece(background,piece_type,
				piece_x,piece_y,piece_rotate);

			for(i=0;i<16;i++) framebuffer[i]=background[i];

			/***********************************/
			/* Clear lines if any are complete */
			/***********************************/
			cleared_lines=0;

			/* count cleared lines */

			for(l=0;l<DISPLAY_SIZE;l++) {
				if (background[l]==0xff) {
					which_lines[cleared_lines]=l;
					cleared_lines++;
				}
			}

			if (cleared_lines) {
				c_effect=&effects[2];
				c_effect_count=0;

				/* Blink the lines */
				for(l=0;l<cleared_lines;l++) {
					for(k=0;k<cleared_lines;k++) {
						framebuffer[which_lines[k]]=0xff;
					}
					update_our_display(display_type,framebuffer);
					usleep(100000);
					for(k=0;k<cleared_lines;k++) {
						framebuffer[which_lines[k]]=0x00;
					}
					update_our_display(display_type,framebuffer);
					usleep(100000);
					for(k=0;k<cleared_lines;k++) {
						framebuffer[which_lines[k]]=0xff;
					}
					update_our_display(display_type,framebuffer);
					usleep(100000);

				}

				/* clear the lines */
				for(l=0;l<DISPLAY_SIZE;l++) {
					if (framebuffer[l]==0xff) {
						for(k=l;k>0;k--) {
							background[k]=background[k-1];
						}
						background[0]=0x00;
					}
				}



				/* Adjust score */
				switch (cleared_lines) {
					case 1:	score+=50*level; break;
					case 2: score+=125*level; break;
					case 3: score+=250*level; break;
					case 4:	score+=500*level; break;
				}

				/* Advance level */
				if (((lines+cleared_lines)/10)>(lines/10)) {
					level++;
				}
				lines+=cleared_lines;

			}

			display_keypad_clear(display_type);

			piece_y=0;
			piece_x=3;
			piece_type=next_piece;
			next_piece=Random_Generator();
			piece_rotate=0;
			score+=level;

			update_aux_display(next_piece,lines);

		}

		/* Calculate time we were busy this frame */
                gettimeofday(&next,NULL);
                s=start.tv_sec+(start.tv_usec/1000000.0);
                n=next.tv_sec+(next.tv_usec/1000000.0);
                diff=(n-s)*1000000.0;

		/* Delay until time for next update, 50Hz */
                if (play_music) {
                        if (diff>0) bcm2835_delayMicroseconds(20000-diff);
                        /* often 50Hz = 20000 */
                        /* TODO: calculate correctly */
                }
                else {
                        if (1) usleep(1000000/ym_song.frame_rate);
                }

                gettimeofday(&next,NULL);
                n=next.tv_sec+(next.tv_usec/1000000.0);
                start.tv_sec=next.tv_sec;
                start.tv_usec=next.tv_usec;


		frame++;
		if (frame>ym_song.num_frames) {
			frame=ym_song.loop_frame;
		}

//		/* 30 frames per second? */
//		usleep(33000);
	}

	if (!done) {
		sleep(2);
		goto start;
	}


	/* Quiet down the chips */
	if (play_music) {
		quiet_ay_3_8910(shift_size);
		close_ay_3_8910();
		max98306_free();
	}

	/* Clear out display */
	display_shutdown(display_type);

	return 0;

}
