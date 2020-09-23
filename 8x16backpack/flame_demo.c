/* ASCII Flame code */
/* Loosely Based on the java-script code
   from http://maettig.com/code/javascript/asciifire.html
   by Thiemo Mättig
*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include "i2c_lib.h"
#include "graphics_lib.h"

#define XSIZE 18
#define YSIZE 10
#define SEEDS 1
#define THRESH 8
#define SLEEP 100000

int b[XSIZE][YSIZE];

static int debug=0;

void animate_flame(void) {

	int i,x,y;
	char a[XSIZE*(YSIZE+2)];
	int n=0;


	/* Set 10 random locations in second-to-last row to be 70 */
	for (i = 0; i < SEEDS; i++) {
		b[ rand() % XSIZE][YSIZE-2] = 70;
	}

	n = 0;

	for(y=1;y<YSIZE-1;y++) {
		for(x=1;x<XSIZE-1;x++) {
			/* New value is the average of the 4-pixel block */
			b[x][y] = ((b[x-1][y]+ b[x][y] + b[x+1][y] +
				b[x-1][y+1] + b[x][y+1] + b[x+1][y+1]) / 6);
			a[n]= b[x][y]>THRESH?'*':' ';
			n++;
		}
		a[n]='\n';
		n++;

		if (debug) {
			printf("%c[2J",27); /* ANSI clear screen */
			printf("%s",a);     /* Print framebuffer */
		}
	}
}


int flame_demo(int i2c_fd) {

	int x,y;
	unsigned short display_buffer[DISPLAY_LINES];

	/* Initialize Framebuffer */
	for(y=0; y<YSIZE+1; y++) {
		for(x=0;x<XSIZE;x++) {
			b[x][y]=0;
		}
	}

	while(1) {
		for (y=0;y<8;y++) display_buffer[y]=0;
		animate_flame();
		for(y=1;y<9;y++) {
			for(x=1;x<17;x++) {
				if (b[x][y]>THRESH) {
					plotxy_16(display_buffer,x-1,y-1);
				}
			}
		}
		update_8x16_display(i2c_fd,HT16K33_ADDRESS0,
			display_buffer,0,BROKEN);
		usleep(SLEEP);
	}

	return 0;
}


int main(int argc, char **argv) {

	int result;
	int i2c_fd;

	i2c_fd=init_i2c("/dev/i2c-1");
	if (i2c_fd < 0) {
		fprintf(stderr,"Error opening device!\n");
		return -1;
	}

	/* Init display */
	if (init_display(i2c_fd,HT16K33_ADDRESS0,10)) {
		fprintf(stderr,"Error opening display\n");
		return -1;
	}

	result=flame_demo(i2c_fd);

	return result;
}

