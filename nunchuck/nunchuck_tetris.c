/* Falling tetromino game */

/* Needs two 8x8 displays and a wii nunchuck */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include <errno.h>

#include <sys/ioctl.h>

#include <linux/i2c-dev.h>

#include "i2c_lib.h"

#define DISPLAY_SIZE	16

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
	int xsize;
	int ysize;
	int x_left_edge;
	int x_right_edge;
	int y_top_edge;
	int y_bottom_edge;
} piece_info[NUM_PIECES][ROTATIONS] = {

	/* O */
	{
		/* Start */
		{
			.xsize=4,
			.ysize=4,
			.x_left_edge=1,
			.x_right_edge=1,
			.y_top_edge=1,
			.y_bottom_edge=1,
		},
		/* 90 CW */
		{
			.xsize=4,
			.ysize=4,
			.x_left_edge=1,
			.x_right_edge=1,
			.y_top_edge=1,
			.y_bottom_edge=1,
		},

		/* 180 CW */
		{
			.xsize=4,
			.ysize=4,
			.x_left_edge=1,
			.x_right_edge=1,
			.y_top_edge=1,
			.y_bottom_edge=1,
		},

		/* 270 CW */
		{
			.xsize=4,
			.ysize=4,
			.x_left_edge=1,
			.x_right_edge=1,
			.y_top_edge=1,
			.y_bottom_edge=1,
		},
	},

	/* I */
	{
		/* Start */
		{
			.xsize=4,
			.ysize=4,
			.x_left_edge=0,
			.x_right_edge=0,
			.y_top_edge=1,
			.y_bottom_edge=2,
		},
		/* 90 CW */
		{
			.xsize=4,
			.ysize=4,
			.x_left_edge=2,
			.x_right_edge=1,
			.y_top_edge=0,
			.y_bottom_edge=0,
		},

		/* 180 CW */
		{
			.xsize=4,
			.ysize=4,
			.x_left_edge=0,
			.x_right_edge=0,
			.y_top_edge=2,
			.y_bottom_edge=1,
		},

		/* 270 CW */
		{
			.xsize=4,
			.ysize=4,
			.x_left_edge=1,
			.x_right_edge=2,
			.y_top_edge=0,
			.y_bottom_edge=0,
		},
	},


	/* T */
	{
		/* Start */
		{
			.xsize=4,
			.ysize=4,
			.x_left_edge=0,
			.x_right_edge=1,
			.y_top_edge=0,
			.y_bottom_edge=2,
		},
		/* 90 CW */
		{
			.xsize=4,
			.ysize=4,
			.x_left_edge=1,
			.x_right_edge=1,
			.y_top_edge=0,
			.y_bottom_edge=1,
		},

		/* 180 CW */
		{
			.xsize=4,
			.ysize=4,
			.x_left_edge=0,
			.x_right_edge=1,
			.y_top_edge=1,
			.y_bottom_edge=1,
		},

		/* 270 CW */
		{
			.xsize=4,
			.ysize=4,
			.x_left_edge=0,
			.x_right_edge=2,
			.y_top_edge=0,
			.y_bottom_edge=1,
		},
	},


	/* S */
	{
		/* Start */
		{
			.xsize=4,
			.ysize=4,
			.x_left_edge=0,
			.x_right_edge=1,
			.y_top_edge=0,
			.y_bottom_edge=2,
		},
		/* 90 CW */
		{
			.xsize=4,
			.ysize=4,
			.x_left_edge=1,
			.x_right_edge=1,
			.y_top_edge=0,
			.y_bottom_edge=1,
		},

		/* 180 CW */
		{
			.xsize=4,
			.ysize=4,
			.x_left_edge=0,
			.x_right_edge=1,
			.y_top_edge=1,
			.y_bottom_edge=1,
		},

		/* 270 CW */
		{
			.xsize=4,
			.ysize=4,
			.x_left_edge=0,
			.x_right_edge=2,
			.y_top_edge=0,
			.y_bottom_edge=1,
		},
	},


	/* Z */
	{
		/* Start */
		{
			.xsize=4,
			.ysize=4,
			.x_left_edge=0,
			.x_right_edge=1,
			.y_top_edge=0,
			.y_bottom_edge=2,
		},
		/* 90 CW */
		{
			.xsize=4,
			.ysize=4,
			.x_left_edge=1,
			.x_right_edge=1,
			.y_top_edge=0,
			.y_bottom_edge=1,
		},

		/* 180 CW */
		{
			.xsize=4,
			.ysize=4,
			.x_left_edge=0,
			.x_right_edge=1,
			.y_top_edge=1,
			.y_bottom_edge=1,
		},

		/* 270 CW */
		{
			.xsize=4,
			.ysize=4,
			.x_left_edge=0,
			.x_right_edge=2,
			.y_top_edge=0,
			.y_bottom_edge=1,
		},
	},

	/* J */
	{
		/* Start */
		{
			.xsize=4,
			.ysize=4,
			.x_left_edge=0,
			.x_right_edge=1,
			.y_top_edge=0,
			.y_bottom_edge=1,
		},
		/* 90 CW */
		{
			.xsize=4,
			.ysize=4,
			.x_left_edge=1,
			.x_right_edge=1,
			.y_top_edge=0,
			.y_bottom_edge=1,
		},

		/* 180 CW */
		{
			.xsize=4,
			.ysize=4,
			.x_left_edge=0,
			.x_right_edge=1,
			.y_top_edge=1,
			.y_bottom_edge=1,
		},

		/* 270 CW */
		{
			.xsize=4,
			.ysize=4,
			.x_left_edge=0,
			.x_right_edge=2,
			.y_top_edge=0,
			.y_bottom_edge=1,
		},
	},


	/* L */
	{
		/* Start */
		{
			.xsize=4,
			.ysize=4,
			.x_left_edge=0,
			.x_right_edge=1,
			.y_top_edge=0,
			.y_bottom_edge=1,
		},
		/* 90 CW */
		{
			.xsize=4,
			.ysize=4,
			.x_left_edge=1,
			.x_right_edge=1,
			.y_top_edge=0,
			.y_bottom_edge=1,
		},

		/* 180 CW */
		{
			.xsize=4,
			.ysize=4,
			.x_left_edge=0,
			.x_right_edge=1,
			.y_top_edge=1,
			.y_bottom_edge=1,
		},

		/* 270 CW */
		{
			.xsize=4,
			.ysize=4,
			.x_left_edge=0,
			.x_right_edge=2,
			.y_top_edge=0,
			.y_bottom_edge=1,
		},
	},

};

int pieces[NUM_PIECES][ROTATIONS][4][4]= {
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
	 {0,0,1,0}},
	{{0,0,0,0},
	 {1,1,1,0},
	 {0,1,0,1},
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
	 {0,0,1,0}},
	{{0,0,0,0},
	 {0,1,1,0},
	 {1,1,1,0},
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
	 {0,0,1,0}},
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
//	display_buffer[piece_y]|=1<<piece_x;

	for(y=0;y<piece_info[which][rotate].ysize;y++) {
		for(x=0;x<piece_info[which][rotate].xsize;x++) {
			if (pieces[which][rotate][y][x])
				display_buffer[(y+piece_y)]|=1<<(x+piece_x);
		}
	}

}

int bottom_collision(unsigned char *framebuffer,
		int which,int piece_x,int piece_y,int rotate) {

	int y_offset;

	y_offset=piece_info[which][rotate].ysize-
		piece_info[which][rotate].y_bottom_edge;

	/* check for floor */
	if (piece_y+y_offset > 15) return 1;

	/* Check for bottom collision */
	if (framebuffer[piece_y+1]&1<<piece_x) return 1;

	/* No collision */
	return 0;
}



int side_collision(unsigned char *framebuffer,
		int which,int piece_x,int piece_y,int rotate) {

	int x_left_offset;
	int x_right_offset;

	x_left_offset=
		piece_info[which][rotate].x_left_edge;

	x_right_offset=piece_info[which][rotate].xsize-
		piece_info[which][rotate].x_right_edge;


	/* check for left wall */
	if (piece_x+x_left_offset<0) return 1;

	/* check for right wall */
	if (piece_x+x_right_offset>8) return 1;

	/* No collision */
	return 0;
}




int main(int arg, char **argv) {

	struct nunchuck_data n_data;
	unsigned char display_buffer[DISPLAY_SIZE];
	unsigned char framebuffer[DISPLAY_SIZE];
	int piece_x=4, piece_y=0,piece_rotate=0,new_piece_x=0;
	int piece_type;
	int l,k;
	int score=0;

	int no_display=1,no_nunchuck=1;

	char *device="/dev/i2c-1";

	int i2c_fd,i,ch;

	/* Init i2c */

	i2c_fd=init_i2c(device);
	if (i2c_fd < 0) {
		fprintf(stderr,"Error opening i2c dev file %s\n",device);
	}
	else {
		/* Init nunchuck */
		no_nunchuck=0;
		if (init_nunchuck(i2c_fd)) {
			fprintf(stderr,"Error initializing nunchuck\n");
			no_nunchuck=1;
		}

		/* Init display */
		no_display=0;
		if (init_display(i2c_fd,HT16K33_ADDRESS1,15)) {
			fprintf(stderr,"Error opening display 1\n");
			no_display=1;
		}
		if (init_display(i2c_fd,HT16K33_ADDRESS2,15)) {
			fprintf(stderr,"Error opening display 2\n");
			no_display=1;
		}

	}


	/* Init keyboard */
	if (init_keyboard()) {
		fprintf(stderr,"Error initializing keyboard\n");
		return -1;
	}

	/* Clear Framebuffer */
	for(i=0;i<DISPLAY_SIZE;i++) framebuffer[i]=0;

	piece_type=Random_Generator();

	while(1) {

		new_piece_x=piece_x;

		/* Read Keyboard */
		ch=read_keyboard();
		if (ch) {
			if ((ch=='q') || (ch=='Q')) break;
			if (ch==KEYBOARD_RIGHT) {
				new_piece_x++;
			}
			if (ch==KEYBOARD_LEFT) {
				new_piece_x--;
			}
			if (ch=='z') {
				piece_rotate--;
			}
			if (ch=='c') {
				piece_rotate++;
			}
		}

		/* Read Nunchuck */

		if (!no_nunchuck) {
			read_nunchuck(i2c_fd,&n_data);

			if (n_data.c_pressed) {
				piece_rotate++;
			}

			if (n_data.z_pressed) {
				piece_rotate--;
			}


			if (n_data.joy_y>140) {
				/* Fast Drop */
				piece_y--;
				if (piece_y<0) piece_y=0;
			}

			if (n_data.joy_y<100) {
				/* slow drop */
				piece_y++;
				if (piece_y>7) piece_y=7;
			}

			if (n_data.acc_x<400) {
				new_piece_x++;
			}

			if (n_data.acc_x>624) {
				new_piece_x--;
			}
		}

		/* Handle rotate overflow */
		if (piece_rotate>3) piece_rotate=0;
		if (piece_rotate<0) piece_rotate=3;

		/* Check side collision */
		if (side_collision(framebuffer,piece_type,
					new_piece_x,piece_y,piece_rotate)) {
			/* do not update */
		}
		else {
			piece_x=new_piece_x;
		}

		/* Copy framebuffer to screen */
		for(i=0;i<DISPLAY_SIZE;i++) {
			display_buffer[i]=framebuffer[i];
		}

		/* Draw Piece */
		draw_piece(display_buffer,piece_type,
			piece_x,piece_y,piece_rotate);

		/* Write Display */
		if (!no_display) {
			update_8x8_display_rotated(i2c_fd,
				HT16K33_ADDRESS2,display_buffer,0);
			update_8x8_display_rotated(i2c_fd,
				HT16K33_ADDRESS1,display_buffer+8,0);

		}
		else {
			emulate_8x16_display(display_buffer);
			printf("Piece %d x %d y %d rotate %d score %d\n",
				piece_type,piece_x,piece_y,piece_rotate,score);
		}

		/* Drop piece! */

		/* check for bottom collision */
		if (bottom_collision(framebuffer,piece_type,piece_x,piece_y,piece_rotate)) {

			/* check if off top */
			if (piece_y==0) {
				printf("GAME OVER!\n");
				printf("Score=%d\n",score);
				break;
			}

			draw_piece(framebuffer,piece_type,
				piece_x,piece_y,piece_rotate);

			/* check if lines complete */
			for(l=0;l<8;l++) {
				if (framebuffer[l]==0xff) {
					for(k=l;k>0;k--) {
						framebuffer[k]=framebuffer[k-1];
					}
					framebuffer[0]=0x00;
					score+=100;
				}
			}

			piece_y=0;
			piece_x=4;
			piece_type=Random_Generator();
			piece_rotate=0;
		}
		else {
			/* move down */
			piece_y++;
		}
		usleep(500000);
	}

	reset_keyboard();

	return 0;

}
