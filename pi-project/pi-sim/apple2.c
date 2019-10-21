#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <SDL.h>

#include "svmwgraph.h"

static unsigned int apple2_color[16]={
	0,		/*  0 black */
	0xe31e60,	/*  1 magenta */
	0x604ebd,	/*  2 dark blue */
	0xff44fd,	/*  3 purple */
	0x00a360,	/*  4 dark green */
	0x9c9c9c,	/*  5 grey 1 */
	0x14cffd,	/*  6 medium blue */
	0xd0c3ff,	/*  7 light blue */
	0x607203,	/*  8 brown */
	0xff6a3c,	/*  9 orange */
	0x9d9d9d,	/* 10 grey 2 */
	0xffa0d0,	/* 11 pink */
	0x14f53c,	/* 12 bright green */
	0xd0dd8d,	/* 13 yellow */
	0x72ffd0,	/* 14 aqua */
	0xffffff,	/* 15 white */
};

void apple2_load_palette(void) {

	int i;

	for(i=0;i<16;i++) {
		red_palette[i]=(apple2_color[i]>>16)&0xff;
		green_palette[i]=(apple2_color[i]>>8)&0xff;
		blue_palette[i]=(apple2_color[i])&0xff;
	}

}


#if 0
static unsigned int apple2_hcolor[8]={
	0,		/*  0 black */
	0x2ad600,
//	0x14f53c,	/*  1 bright green */
//	0xff44fd,	/*  2 purple */
	0xc530ff,
	0xffffff,	/*  3 white */
	0,		/*  4 black */
	0xff6a3c,	/*  5 orange */
	0x14cffd,	/*  6 medium blue */
	0xffffff,	/*  7 white */
};
#endif

/* 40x48 -- 640x480, 16x10 */
void apple2_plot(int x, int y, int color, unsigned char *buffer) {

	int xx,yy;

	for(yy=0;yy<10;yy++) {
		for(xx=0;xx<16;xx++){
			buffer[ (((y*10)+yy)*XSIZE)+ (x*16)+xx]=color;
		}
	}
}


