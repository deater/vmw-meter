#include <stdio.h>
#include <unistd.h>

#include "pi-sim.h"
#include "default_font.h"

int main(int argc, char **argv) {

	int x,ch;
	unsigned char buffer[XSIZE*YSIZE];

	pisim_init();

	for(x=0;x<256;x++) {
		red_palette[x]=0;
		green_palette[x]=0;
		blue_palette[x]=0;
	}

	/* Do the VMW Software Production Logo */
	for(x=0;x<=40;x++) {
		/* red */
		red_palette[100+x]=((x+20)*4);
		green_palette[100+x]=0;
		blue_palette[100+x]=0;

		/* blue */
		red_palette[141+x]=0;
		green_palette[141+x]=0;
		blue_palette[141+x]=((x+20)*4);

		/* green */
		red_palette[182+x]=0;
		green_palette[182+x]=((x+20)*4);
		blue_palette[182+x]=0;
	}

	/* Set the white color */
	red_palette[15]=0xff;
	green_palette[15]=0xff;
	blue_palette[15]=0xff;

	/* Actually draw the stylized VMW */
	for(x=0;x<=40;x++) {
		vlin(45,45+2*x,x+40,100+x,buffer);
		vlin(45,45+2*x,x+120,141+x,buffer);
		vlin(45,45+2*x,x+200,141+x,buffer);
		vlin(125-(2*x),125,x+80,182+x,buffer);
		vlin(125-(2*x),125,x+160,182+x,buffer);
	}
	for(x=40;x>0;x--){
		vlin(45,45+80-(2*x),x+80,140-x,buffer);
		vlin(45,45+80-(2*x),x+160,181-x,buffer);
		vlin(45,45+80-(2*x),x+240,181-x,buffer);
		vlin(45+(2*x),125,x+120,222-x,buffer);
		vlin(45+(2*x),125,x+200,222-x,buffer);
	}

   vmwTextXY("A VMW SOFTWARE PRODUCTION",60,140,
	     15,15,0,default_font,buffer);


	while(1) {

		ch=pisim_input();
		if (ch=='q') break;
		if (ch==27) break;
		pisim_update(buffer);
		usleep(10000);
	}

	vmwFadeToBlack(buffer);

	while(1) {

		ch=pisim_input();
		if (ch=='q') break;
		if (ch==27) break;
		pisim_update(buffer);
		usleep(10000);
	}


	return 0;
}


