#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "svmwgraph.h"

unsigned char red_palette[256];
unsigned char blue_palette[256];
unsigned char green_palette[256];

void vmwHlin(int x1, int x2, int y, int color, unsigned char *buffer) {

	int x,output_pointer;

	output_pointer=(y*XSIZE);

	for(x=x1;x<x2;x++) {
		buffer[output_pointer+x]=color;
	}
}

void vmwVlin(int y1, int y2, int x, int color, unsigned char *buffer) {

	int y,output_pointer;

	output_pointer=(y1*XSIZE)+x;

	for(y=y1;y<y2;y++) {
		buffer[output_pointer]=color;
		output_pointer+=XSIZE;
	}
}


void vmwPlot(int x,int y, int color, unsigned char *buffer) {

	int output_pointer;

	output_pointer=(y*XSIZE)+x;

	buffer[output_pointer]=color;
}


void vmwFadeToBlack(unsigned char *buffer) {

	int i,j;

	for(j=0;j<256;j++) {
		for (i=0;i<256;i++) {
			if (red_palette[i]) red_palette[i]--;
			if (green_palette[i]) green_palette[i]--;
			if (blue_palette[i]) blue_palette[i]--;
		}
		usleep(1000);
		pisim_update(buffer);

	}
}

void vmwClearScreen(int color, unsigned char *buffer) {

	memset(buffer,color,XSIZE*YSIZE*sizeof(unsigned char));

}

void vmwSetAllBlackPalette(void) {

	/* FIXME: use memset? */
	int i;

	for (i=0;i<256;i++) {
		red_palette[i]=0;
		green_palette[i]=0;
		blue_palette[i]=0;
	}

}


void vmwFadeFromBlack(unsigned char *buffer, unsigned char *pal) {

	int i,j;

	for(j=0;j<256;j++) {
		for (i=0;i<256;i++) {
			if (red_palette[i]) red_palette[i]--;
			if (green_palette[i]) green_palette[i]--;
			if (blue_palette[i]) blue_palette[i]--;
		}
		usleep(1000);
		pisim_update(buffer);
	}
}


