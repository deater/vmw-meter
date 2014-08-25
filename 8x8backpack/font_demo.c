/* Prints letters from a font */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "i2c_lib.h"
#include "graphics_lib.h"

unsigned char font[256][8];

int load_font(char *filename) {

	FILE *fff;
	int i,j;
	unsigned char fontblock[16];

	fff=fopen(filename,"r");
	if (fff==NULL) {
		fprintf(stderr,"Error opening %s\n",filename);
		return -1;
	}

	for(i=0;i<256;i++) {
		fread(fontblock,1,16,fff);
		for(j=0;j<8;j++) font[i][j]=fontblock[j];
	}

	fclose(fff);

	return 0;
}

int put_letter(unsigned char letter, int i2c_fd) {

	int i;
	unsigned char display_buffer[8];

	for(i=0;i<8;i++) {
		display_buffer[i]=font[letter][i];
	}
	update_8x8_display_rotated(i2c_fd,HT16K33_ADDRESS0,display_buffer,
			0,BROKEN);

	return 0;
}


int main(int argc, char **argv) {

	int result,i;
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

	result=load_font("tbfont.tb1");

	while(1) {
		for(i=32;i<127;i++) {
			result=put_letter(i,i2c_fd);
			usleep(500000);
		}
	}

	return result;
}

