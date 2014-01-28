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

#include "messages.h"

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
			if (pieces[which][rotate][y][x])
				display_buffer[(y+piece_y)]|=1<<(7-(x+piece_x));
		}
	}

}

int bottom_collision(unsigned char *framebuffer,
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
			if (pieces[which][rotate][y][x])
				if (framebuffer[piece_y+y+1]&
					1<<(7-(piece_x+x))) return 1;
		}
	}

	/* No collision */
	return 0;
}



int side_collision(unsigned char *framebuffer,
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
				if (framebuffer[piece_y+y+1]&
					1<<(7-(piece_x+x))) return 1;
		}
	}



	/* No collision */
	return 0;
}

int num_to_seg(int num) {

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

void update_aux_display(unsigned short *aux_display,int next_piece,int lines) {

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


static void update_our_display(unsigned char *display_buffer, int i2c_fd,int no_display) {

	/* Write Display */
	if (!no_display) {
		update_8x8_display_rotated(i2c_fd,
			HT16K33_ADDRESS1,display_buffer,0);
		update_8x8_display_rotated(i2c_fd,
			HT16K33_ADDRESS0,display_buffer+8,0);
	}
	else {
		emulate_8x16_display(display_buffer);
//		printf("Piece %d x %d y %d rotate %d lines %d level %d score %d\n",
//			piece_type,piece_x,piece_y,piece_rotate,lines,level,score);
//		printf("Next piece: ");
//		switch(next_piece) {
//			case T_O: printf("O"); break;
//			case T_I: printf("I"); break;
//			case T_S: printf("S"); break;
//			case T_Z: printf("Z"); break;
//			case T_J: printf("J"); break;
//			case T_L: printf("L"); break;
//			case T_T: printf("T"); break;
//		}
//		printf("\n");
	}
}


static void clear_keyboard(void) {

	int ch;

	do {
		/* Read Keyboard */
		ch=read_keyboard();
	} while(ch>0);
}


static int repeat_until_keypressed(int i2c_fd, int no_nunchuck, int give_up) {

	int ch;
	int length=0;
	struct nunchuck_data n_data;

	while(1) {

		/* Read Keyboard */
		ch=read_keyboard();
		if (ch>0) {
			return ch;
		}

		/* Read Nunchuck */

		if (!no_nunchuck) {
			read_nunchuck(i2c_fd,&n_data);

			if (n_data.c_pressed) {
				return 'c';
			}

			if (n_data.z_pressed) {
				return 'z';
			}
		}

		length++;
		if (length>give_up) return 0;

		usleep(33000);
	}
	return 0;
}


int main(int arg, char **argv) {

	struct nunchuck_data n_data;
	unsigned char display_buffer[DISPLAY_SIZE];
	unsigned char framebuffer[DISPLAY_SIZE];
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

	int z_down=0;
	int c_down=0;

	int no_display=1,no_nunchuck=1,no_aux_display=1;

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
		if (init_display(i2c_fd,HT16K33_ADDRESS0,15)) {
			fprintf(stderr,"Error opening display 1\n");
			no_display=1;
		}
		if (init_display(i2c_fd,HT16K33_ADDRESS1,15)) {
			fprintf(stderr,"Error opening display 2\n");
			no_display=1;
		}
		/* Init aux display */
		no_aux_display=0;
		if (init_display(i2c_fd,HT16K33_ADDRESS2,15)) {
			fprintf(stderr,"Error opening aux display\n");
			no_aux_display=1;
		}
	}


	/* Init keyboard */
	if (init_keyboard()) {
		fprintf(stderr,"Error initializing keyboard\n");
		return -1;
	}

start:

	clear_keyboard();

	/* Display "Hit C" */
	for(i=0;i<DISPLAY_SIZE;i++) display_buffer[i]=hit_c[i];
	update_our_display(display_buffer,i2c_fd,no_display);

	repeat_until_keypressed(i2c_fd,no_nunchuck,30*33);

	/* Clear Framebuffer */
	for(i=0;i<DISPLAY_SIZE;i++) framebuffer[i]=0;

	piece_type=Random_Generator();
	next_piece=Random_Generator();

	update_aux_display(aux_buffer,next_piece,lines);


	while(1) {

		new_piece_x=piece_x;
		new_rotate=piece_rotate;

		/* Read Keyboard */
		ch=read_keyboard();
		if (ch) {
			if ((ch=='q') || (ch=='Q')) {
				reset_keyboard();
				return 0;
			}
			if (ch==KEYBOARD_RIGHT) {
				new_piece_x++;
			}
			if (ch==KEYBOARD_LEFT) {
				new_piece_x--;
			}
			if (ch==KEYBOARD_DOWN) {
				piece_y++;
				score+=level;
			}
			if (ch==KEYBOARD_UP) {
			}
			if (ch=='z') {
				new_rotate--;
			}
			if (ch=='c') {
				new_rotate++;
			}
		}

		/* Read Nunchuck */

		if (!no_nunchuck) {
			read_nunchuck(i2c_fd,&n_data);

			if (n_data.c_pressed) {
				if (!c_down) {
					new_rotate++;
				}
				c_down++;

				if (c_down>10) c_down=0;
			}
			else {
				c_down=0;
			}


			if (n_data.z_pressed) {
				if (!z_down) {
					new_rotate--;
				}
				z_down++;
				if (z_down>10) z_down=0;
			}
			else {
				z_down=0;
			}

			if (n_data.joy_y>140) {
				/* Fast Drop */
				piece_y--;
				if (piece_y<0) piece_y=0;
			}

			if (n_data.joy_y<100) {
				/* slow drop */
				piece_y++;
				score+=level;
			}

			if (n_data.acc_y<400) {
				piece_y++;
				score+=level;
			}

			if (n_data.acc_x<400) {
				new_piece_x--;
			}

			if (n_data.acc_x>624) {
				new_piece_x++;
			}
		}

		/* Handle rotate overflow */
		if (new_rotate>3) new_rotate=0;
		if (new_rotate<0) new_rotate=3;

		/* Check side collision */
		if (side_collision(framebuffer,piece_type,
					new_piece_x,piece_y,new_rotate)) {
			/* do not update */
		}
		else {
			piece_x=new_piece_x;
			piece_rotate=new_rotate;
		}

		/* Copy framebuffer to screen */
		for(i=0;i<DISPLAY_SIZE;i++) {
			display_buffer[i]=framebuffer[i];
		}

		/* Draw Piece */
		draw_piece(display_buffer,piece_type,
			piece_x,piece_y,piece_rotate);


		update_our_display(display_buffer,i2c_fd,no_display);

		if (!no_aux_display) {
			update_display(i2c_fd,
				HT16K33_ADDRESS2,aux_buffer);
		}
		else {
			emulate_4x7seg_display(aux_buffer);
		}


		/* Gravity */
		fractional_y+=level;
		if (fractional_y>33) {
			fractional_y-=33;
			piece_y++;
		}

		/* check for bottom collision */
		if (bottom_collision(framebuffer,piece_type,piece_x,
					piece_y,piece_rotate)) {

			/* check if off top */
			if (piece_y==0) {
				printf("GAME OVER!\n");
				printf("Score=%d\n",score);
				for(i=0;i<DISPLAY_SIZE;i++) {
					display_buffer[i]=0xff;
					update_our_display(display_buffer,i2c_fd,no_display);
					usleep(100000);
				}
				/* Display "Game" */
				for(i=0;i<DISPLAY_SIZE;i++) display_buffer[i]=game[i];
				update_our_display(display_buffer,i2c_fd,no_display);
				usleep(500000);

				/* Display "Over" */
				for(i=0;i<DISPLAY_SIZE;i++) display_buffer[i]=over[i];
				update_our_display(display_buffer,i2c_fd,no_display);

				level=1;
				lines=0;

				break;
			}

			draw_piece(framebuffer,piece_type,
				piece_x,piece_y,piece_rotate);

			/***********************************/
			/* Clear lines if any are complete */
			/***********************************/
			cleared_lines=0;

			/* count cleared lines */

			for(l=0;l<DISPLAY_SIZE;l++) {
				if (framebuffer[l]==0xff) {
					which_lines[cleared_lines]=l;
					cleared_lines++;
				}
			}

			if (cleared_lines) {

				/* Blink the lines */
				for(l=0;l<cleared_lines;l++) {
					for(k=0;k<cleared_lines;k++) {
						display_buffer[which_lines[k]]=0xff;
					}
					update_our_display(display_buffer,i2c_fd,no_display);
					usleep(100000);
					for(k=0;k<cleared_lines;k++) {
						display_buffer[which_lines[k]]=0x00;
					}
					update_our_display(display_buffer,i2c_fd,no_display);
					usleep(100000);
					for(k=0;k<cleared_lines;k++) {
						display_buffer[which_lines[k]]=0xff;
					}
					update_our_display(display_buffer,i2c_fd,no_display);
					usleep(100000);

				}

				/* clear the lines */
				for(l=0;l<DISPLAY_SIZE;l++) {
					if (framebuffer[l]==0xff) {
						for(k=l;k>0;k--) {
							framebuffer[k]=framebuffer[k-1];
						}
						framebuffer[0]=0x00;
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

			clear_keyboard();
			piece_y=0;
			piece_x=3;
			piece_type=next_piece;
			next_piece=Random_Generator();
			piece_rotate=0;
			score+=level;

			update_aux_display(aux_buffer,next_piece,lines);

		}

		/* 30 frames per second? */
		usleep(33000);
	}

	sleep(2);

	goto start;


	return 0;

}
