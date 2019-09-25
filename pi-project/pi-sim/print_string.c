#include <stdio.h>	/* For FILE I/O */
#include <string.h>	/* For strncmp */
#include <fcntl.h>	/* for open()  */
#include <unistd.h>	/* for lseek() */
#include <sys/stat.h>	/* for file modes */
#include <stdlib.h>	/* free() */

#include "pi-sim.h"

#include "default_font.h"

#define FONTSIZE_Y	16
#define	FONTSIZE_X	8

int put_char(unsigned char c, int x, int y, int color) {
	int xx,yy;

	int output_pointer;

	output_pointer=(y*XSIZE)+x;

	for(yy=0;yy<FONTSIZE_Y;yy++) {
		for(xx=0;xx<FONTSIZE_X;xx++) {
			if (default_font[c][yy]&(1<<(FONTSIZE_X-xx))) {
				red[output_pointer]=red_palette[color];
				green[output_pointer]=green_palette[color];
				blue[output_pointer]=blue_palette[color];
			}
			output_pointer++;
		}
		output_pointer+=(XSIZE-FONTSIZE_X);
	}
	return 0;

}

int print_string(char *string, int x, int y, int color)  {

	int i;
//	printf("Putting %d at %d,%d,%d\n",string[0],x,y,color);
	for(i=0;i<strlen(string);i++) {
		put_char(string[i],x+(i*FONTSIZE_X),y,color);
	}

	return 0;
}

