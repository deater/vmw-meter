/* based on info at https://www.cl.cam.ac.uk/projects/raspberrypi/tutorials/os/screen01.html */
/* http://elinux.org/RPi_Framebuffer */

/* Note if you end up with Blue and Red switched */
/* You need to update the firmware on your Pi    */

#include <stddef.h>
#include <stdint.h>

#include "printk.h"
#include "mmio.h"

#include "mailbox.h"
#include "framebuffer.h"

#include "string.h"

static uint32_t debug=1;

static int framebuffer_initialized=0;

static struct frame_buffer_info_type current_fb;
static unsigned char offscreen[2048*2048*3];

struct frame_buffer_info_type {
	int phys_x,phys_y;	/* IN: Physical Width / Height*/
	int virt_x,virt_y;	/* IN: Virtual Width / Height */
	int pitch;		/* OUT: bytes per row */
	int depth;		/* IN: bits per pixel */
	int x,y;		/* IN: offset to skip when copying fb */
	int pointer;		/* OUT: pointer to the framebuffer */
	int size;		/* OUT: size of the framebuffer */
};


int framebuffer_ready(void) {

	return framebuffer_initialized;

}

static void dump_framebuffer_info(struct frame_buffer_info_type *fb) {

	printk("px %d py %d vx %d vy %d pitch %d depth %d x %d y %d ptr %x sz %d\n",
		fb->phys_x,fb->phys_y,
		fb->virt_x,fb->virt_y,
		fb->pitch,fb->depth,
		fb->x,fb->y,
		fb->pointer,fb->size);

	return;
}

static struct frame_buffer_info_type fb_info  __attribute__ ((aligned(16)));;

char *framebuffer_init(int x, int y, int depth) {



	int result;

	fb_info.phys_x=x;
	fb_info.phys_y=y;
	fb_info.virt_x=x;
	fb_info.virt_y=y;
	fb_info.pitch=0;
	fb_info.depth=depth;
	fb_info.x=0;
	fb_info.y=0;
	fb_info.pointer=0;
	fb_info.size=0;

	if (debug) {
		printk("fb: Writing message @%x to mailbox %x\n",
			&fb_info,MAILBOX_BASE);
		printk("fb: asking for ");
		dump_framebuffer_info(&fb_info);
	}

	result=mailbox_write( (unsigned int)(&fb_info)+0x40000000 ,
		MAILBOX_CHAN_FRAMEBUFFER);

	if (result<0) {
		printk("Mailbox write failed\n");
		return NULL;
	}

	result=mailbox_read(MAILBOX_CHAN_FRAMEBUFFER);

	if (debug) {
		printk("fb: we got ");
		dump_framebuffer_info(&fb_info);
	}

	if (result==-1) {
		printk("Mailbox read failed\n");
		return NULL;
	}

	current_fb.pointer=(int)(fb_info.pointer);
	current_fb.phys_x=fb_info.phys_x;
	current_fb.phys_y=fb_info.phys_y;
	current_fb.pitch=fb_info.pitch;
	current_fb.depth=fb_info.depth;

	framebuffer_initialized=1;

	return (char *)(fb_info.pointer);
}

int framebuffer_hline(int color, int x0, int x1, int y) {

	int x;
	int r,g,b;

	unsigned char *fb=(char *)(fb_info.pointer);

	r=(color&0xff0000)>>16;
	g=(color&0x00ff00)>>8;
	b=color&0x0000ff;

	for(x=x0;x<x1;x++) {
		fb[(y*current_fb.pitch)+(x*3)+0]=r;
		fb[(y*current_fb.pitch)+(x*3)+1]=g;
		fb[(y*current_fb.pitch)+(x*3)+2]=b;
	}

	return 0;
}

int framebuffer_vline(int color, int y0, int y1, int x) {

	int y;
	int r,g,b;

	unsigned char *fb=(char *)(fb_info.pointer);

	r=(color&0xff0000)>>16;
	g=(color&0x00ff00)>>8;
	b=color&0x0000ff;

	for(y=y0;y<y1;y++) {
		fb[(y*current_fb.pitch)+(x*3)+0]=r;
		fb[(y*current_fb.pitch)+(x*3)+1]=g;
		fb[(y*current_fb.pitch)+(x*3)+2]=b;
	}

	return 0;
}

int framebuffer_clear_screen(int color) {

	int y;

	for(y=0;y<current_fb.phys_y;y++) {
		framebuffer_hline(color,0,current_fb.phys_x,y);
	}

	return 0;
}

int framebuffer_putpixel(int color, int x, int y) {

	int r,g,b;

	unsigned char *fb=(char *)(fb_info.pointer);

	r=(color&0xff0000)>>16;
	g=(color&0x00ff00)>>8;
	b=color&0x0000ff;

	fb[(y*current_fb.pitch)+(x*3)+0]=r;
	fb[(y*current_fb.pitch)+(x*3)+1]=g;
	fb[(y*current_fb.pitch)+(x*3)+2]=b;

	return 0;

}

int framebuffer_gradient(void) {

	int x;

	for(x=0;x<current_fb.phys_x;x++) {
		framebuffer_vline( (x*256)/800,
				/* hardcoded, can't divide? */
				0,current_fb.phys_y-1, x);
	}

	return 0;
}
