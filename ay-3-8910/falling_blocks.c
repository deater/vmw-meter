/* Falling tetromino game */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>

#include <errno.h>

#include <sys/resource.h>

#include "i2c_lib.h"
#include "display.h"
#include "ay-3-8910.h"
#include "max98306.h"


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
				display_8x16_vertical_putpixel(display_buffer,
					x+piece_x,y+piece_y);

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
	if (piece_y+y_offset > 15) return 1;

	/* Check for bottom collision */
	for(x=piece_info[which][rotate].x_left_edge;
			x<SPRITE_SIZE-piece_info[which][rotate].x_right_edge;
			x++) {
		for(y=y_offset-1;y>=0;y--) {
			if (pieces[which][rotate][y][x]) {
				if (display_8x16_vertical_getpixel(
					framebuffer,piece_x+x,piece_y+y+1))
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
				if (display_8x16_vertical_getpixel(
					framebuffer,piece_x+x,piece_y+y+1))
						return 1;
//				if (framebuffer[piece_y+y+1]&
//					1<<(7-(piece_x+x))) return 1;
		}
	}

	/* check if right edges are in a tower */
	for(y=piece_info[which][rotate].y_top_edge;
			y<SPRITE_SIZE-piece_info[which][rotate].y_bottom_edge;
			y++) {
		for(x=x_left_offset;x<SPRITE_SIZE;x++) {
			if (pieces[which][rotate][y][x])
				if (display_8x16_vertical_getpixel(
					framebuffer,piece_x+x,piece_y+y+1))
						return 1;
//				if (framebuffer[piece_y+y+1]&
//					1<<(7-(piece_x+x))) return 1;
		}
	}



	/* No collision */
	return 0;
}

static int num_to_seg(int num) {

	switch(num) {
		case 0:	return 0x3f;
		case 1: return 0x06;
		case 2: return 0x5b;
		case 3: return 0x4f;
		case 4: return 0x66;
		case 5: return 0x6d;
		case 6: return 0x7d;
		case 7: return 0x07;
		case 8: return 0x7f;
		case 9: return 0x67;
	}

	return 0;
}

static void update_aux_display(unsigned short *aux_display,int next_piece,int lines) {

	int i;
	int hundreds,tens,ones;

	for(i=0;i<8;i++) {
		aux_display[i]=0;
	}

	switch(next_piece) {
		case T_O: aux_display[0]=0x63; break;
		case T_I: aux_display[0]=0x06; break;
		case T_T: aux_display[0]=0x46; break;
		case T_J: aux_display[0]=0x0e; break;
		case T_L: aux_display[0]=0x38; break;
		case T_S: aux_display[0]=0x64; break;
		case T_Z: aux_display[0]=0x52; break;
	}

	hundreds=lines/100;
	tens=(lines-(hundreds*100))/10;
	ones=lines%10;

	if (hundreds) aux_display[1]=num_to_seg(hundreds);
	if ((tens) || ((hundreds) && (!tens))) aux_display[3]=num_to_seg(tens);
	aux_display[4]=num_to_seg(ones);
}


static void update_our_display(int display_type, unsigned char *framebuffer) {

	unsigned char buffer[17];
	int i;

//		update_8x8_display_rotated(i2c_fd,
//			HT16K33_ADDRESS1,display_buffer,0,BROKEN);


	buffer[0]=0;
	for(i=0;i<16;i++) buffer[i+1]=framebuffer[i];

	display_8x16_raw(display_type,buffer);

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

int main(int arg, char **argv) {

	unsigned char framebuffer[DISPLAY_SIZE];
	unsigned char background[DISPLAY_SIZE];


	unsigned short aux_buffer[8];
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

	/* Setup control-C handler to quiet the music   */
	/* otherwise if you force quit it keeps playing */
	/* the last tones */
	signal(SIGINT, quiet_and_exit);

	/* Set to have highest possible priority */
	setpriority(PRIO_PROCESS, 0, -20);

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



start:

	display_keypad_clear(display_type);

	/* "Hit a Key" */
	sprintf(text_string,"%s","  HIT A KEY ");
	display_14seg_string(display_type,text_string);

	display_keypad_repeat_until_keypressed(display_type);

	/* Clear Framebuffer */
	for(i=0;i<DISPLAY_SIZE;i++) framebuffer[i]=0;

	piece_type=Random_Generator();
	next_piece=Random_Generator();

	update_aux_display(aux_buffer,next_piece,lines);

	for(i=0;i<16;i++) {
		framebuffer[i]=0;
		background[i]=0;
	}

	while(1) {

		new_piece_x=piece_x;
		new_rotate=piece_rotate;

		/* Read Keyboard */
		ch=display_keypad_read(display_type);
		if (ch) {
			if (ch==CMD_EXIT_PROGRAM) {
				done=1;
				break;
			}
			if (ch==CMD_FF) { // '.'
				new_piece_x++;
			}
			if (ch==CMD_RW) { // ','
				new_piece_x--;
			}
			if (ch==CMD_STOP) { // 's'
				piece_y++;
				score+=level;
			}
			if (ch==CMD_PLAY) { // ' '
				new_rotate--;
			}
			if (ch==CMD_MENU) { // 'm'
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

			/* check if off top */
			if (piece_y==0) {
				printf("GAME OVER!\n");
				printf("Score=%d\n",score);
				for(i=0;i<DISPLAY_SIZE;i++) {
					framebuffer[i]=0xff;
					update_our_display(display_type,framebuffer);
					usleep(100000);
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

			// update_aux_display(aux_buffer,next_piece,lines);

		}

		/* 30 frames per second? */
		usleep(33000);
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
