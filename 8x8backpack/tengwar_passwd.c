/* Tolkien sillyness */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include <errno.h>

#include <sys/ioctl.h>

#include <linux/i2c-dev.h>

#include "i2c_lib.h"

#include "tengwar_messages.h"

#define DISPLAY_SIZE	16
#define SPRITE_SIZE	4

static void update_our_display(unsigned char *display_buffer, int i2c_fd,int no_display) {

	/* Write Display */
	if (!no_display) {
		update_8x8_display_rotated(i2c_fd,
			HT16K33_ADDRESS2,display_buffer,0);
		update_8x8_display_rotated(i2c_fd,
			HT16K33_ADDRESS1,display_buffer+8,0);
	}
	else {
		emulate_8x16_display(display_buffer);
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

	while(1) {

		/* Read Keyboard */
		ch=read_keyboard();
		if (ch>0) {
			return ch;
		}


		length++;
		if (length>give_up) return 0;

		usleep(33000);
	}
	return 0;
}

#define FRAME_WIDTH	1024


int main(int arg, char **argv) {


	unsigned char framebuffer[FRAME_WIDTH];

	int no_display=1,no_nunchuck=1;

	char *device="/dev/i2c-1";

	int i2c_fd,i;

	/* Init i2c */

	i2c_fd=init_i2c(device);
	if (i2c_fd < 0) {
		fprintf(stderr,"Error opening i2c dev file %s\n",device);
	}
	else {
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

	clear_keyboard();

	/* clear framebuffer */
	for(i=0;i<FRAME_WIDTH;i++) framebuffer[i]=0;

	/***************************/
	/* DISPLAY STARS OF FEANOR */
	/***************************/

	/* copy string to scroll to framebuffer */
	for(i=0;i<sizeof(f_star);i++) {
		framebuffer[i]=f_star[i];
	}
	update_our_display(framebuffer,i2c_fd,no_display);

	sleep(2);

	/* clear framebuffer */
	for(i=0;i<FRAME_WIDTH;i++) framebuffer[i]=0;

	/***************************/
	/* DISPLAY MADE_BY_VINCE   */
	/***************************/

	/* Im Vince hain echant. */

	/* copy string to scroll to framebuffer */
	for(i=0;i<sizeof(im_vince);i++) {
		framebuffer[(FRAME_WIDTH-16)-i]=im_vince[(sizeof(im_vince)-1)-i];
	}

	/* scroll */
	for(i=FRAME_WIDTH-16;i>=(FRAME_WIDTH-sizeof(im_vince))-32;i--) {
		update_our_display(framebuffer+i,i2c_fd,no_display);
		usleep(100000);
	}

	/* clear framebuffer */
	for(i=0;i<FRAME_WIDTH;i++) framebuffer[i]=0;

	/***************************/
	/* DISPLAY GATE PHRASE     */
	/***************************/

	/* pedo mellon a minno! */

	/* copy string to scroll to framebuffer */
	for(i=0;i<sizeof(mellon);i++) {
		framebuffer[(FRAME_WIDTH-16)-i]=mellon[(sizeof(mellon)-1)-i];
	}

	/* scroll */
	for(i=FRAME_WIDTH-16;i>=(FRAME_WIDTH-sizeof(mellon))-32;i--) {
		update_our_display(framebuffer+i,i2c_fd,no_display);
		usleep(100000);
	}

	sleep(1);


	/* clear framebuffer */
	for(i=0;i<FRAME_WIDTH;i++) framebuffer[i]=0;

	/***************************/
	/* DISPLAY NAME:           */
	/***************************/

	/* ess\"e */

	/* copy string to scroll to framebuffer */
	for(i=0;i<sizeof(esse);i++) {
		framebuffer[(FRAME_WIDTH-16)-i]=esse[(sizeof(esse)-1)-i];
	}

	/* scroll */
	for(i=FRAME_WIDTH-16;i>=(FRAME_WIDTH-sizeof(esse)-16);i--) {
		update_our_display(framebuffer+i,i2c_fd,no_display);
		usleep(100000);
	}

	/* Wait for keypress */

	repeat_until_keypressed(i2c_fd,no_nunchuck,30*33);

	/***************************/
	/* VINS:                   */
	/***************************/

	int scroll=0;
	int scroll_amounts[]={7,10,21,29};
	int frame_shift=0,old_frame_shift=0;

	while(1) {

		/* copy string to scroll to framebuffer */
		for(i=0;i<scroll_amounts[scroll]+1;i++) {
			framebuffer[(FRAME_WIDTH)-i]=vins[(sizeof(vins))-i];
		}

		if (scroll_amounts[scroll]>16) frame_shift=scroll_amounts[scroll]-16;

		for(i=old_frame_shift;i<=frame_shift;i++) {
			update_our_display(framebuffer+(FRAME_WIDTH-16-i),i2c_fd,no_display);
			usleep(100000);
		}
		old_frame_shift=frame_shift;

		repeat_until_keypressed(i2c_fd,no_nunchuck,30*33);
		scroll++;
		if (scroll>3) break;
	}


	/* clear framebuffer */
	for(i=0;i<FRAME_WIDTH;i++) framebuffer[i]=0;


	/***************************/
	/* avanierquetta:          */
	/***************************/

	/* copy string to scroll to framebuffer */
	for(i=0;i<sizeof(avanierquetta);i++) {
		framebuffer[(FRAME_WIDTH-16)-i]=avanierquetta[(sizeof(avanierquetta)-1)-i];
	}

	/* scroll */
	for(i=FRAME_WIDTH-16;i>=(FRAME_WIDTH-sizeof(avanierquetta)-16);i--) {
		update_our_display(framebuffer+i,i2c_fd,no_display);
		usleep(100000);
	}

	repeat_until_keypressed(i2c_fd,no_nunchuck,30*33);

	/* clear framebuffer */
	for(i=0;i<FRAME_WIDTH;i++) framebuffer[i]=0;


	/***************************/
	/* FREND:                  */
	/***************************/

	/***************************/
	/* VINS:                   */
	/***************************/

	scroll=0;
	scroll_amounts[0]=7;
	scroll_amounts[1]=16;
	scroll_amounts[2]=23;
	scroll_amounts[3]=33;
	frame_shift=0,old_frame_shift=0;

	while(1) {

		/* copy string to scroll to framebuffer */
		for(i=0;i<scroll_amounts[scroll]+1;i++) {
			framebuffer[(FRAME_WIDTH)-i]=frend[(sizeof(frend))-i];
		}

		if (scroll_amounts[scroll]>16) frame_shift=scroll_amounts[scroll]-16;

		for(i=old_frame_shift;i<=frame_shift;i++) {
			update_our_display(framebuffer+(FRAME_WIDTH-16-i),i2c_fd,no_display);
			usleep(100000);
		}
		old_frame_shift=frame_shift;

		repeat_until_keypressed(i2c_fd,no_nunchuck,30*33);
		scroll++;
		if (scroll>3) break;
	}

	/* clear framebuffer */
	for(i=0;i<FRAME_WIDTH;i++) framebuffer[i]=0;


	/***************************/
	/* va'quetin:              */
	/***************************/

	/* copy string to scroll to framebuffer */
	for(i=0;i<sizeof(vaquetin);i++) {
		framebuffer[(FRAME_WIDTH-16)-i]=vaquetin[(sizeof(vaquetin)-1)-i];
	}

	/* scroll */
	for(i=FRAME_WIDTH-16;i>=(FRAME_WIDTH-sizeof(vaquetin)-16);i--) {
		update_our_display(framebuffer+i,i2c_fd,no_display);
		usleep(100000);
	}

	repeat_until_keypressed(i2c_fd,no_nunchuck,30*33);

	/* clear framebuffer */
	for(i=0;i<FRAME_WIDTH;i++) framebuffer[i]=0;


	sleep(2);

	reset_keyboard();
        /* restore default colors */
	printf("\x1b[0m");

	return 0;

}
