/* Makes a clock */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include "i2c_lib.h"
#include "graphics_lib.h"


#define DIGIT_WIDTH 4
#define DIGIT_HEIGHT 5

struct digit_type {
  unsigned char line[5];
} digits[11] = {
{{
  0xe, // ###
  0xa, // # #
  0xa, // # #
  0xa, // # #
  0xe, // ###
}}, {{
  0x4, //  #
  0x4, //  #
  0x4, //  #
  0x4, //  #
  0x4, //  #
}}, {{
  0xe, // ###
  0x2, //   #
  0xe, // ###
  0x8, // #
  0xe, // ###
}}, {{
  0xe, // ###
  0x2, //   #
  0x6, //  ##
  0x2, //   #
  0xe, // ###
}}, {{
  0xa, // # #
  0xa, // # #
  0xe, // ###
  0x2, //   #
  0x2, //   #
}}, {{
  0xe, // ###
  0x8, // #
  0xe, // ###
  0x2, //   #
  0xe, // ###
}}, {{
  0xe, // ###
  0x8, // #
  0xe, // ###
  0xa, // # #
  0xe, // ###
}}, {{
  0xe, // ###
  0x2, //   #
  0x2, //   #
  0x2, //   #
  0x2, //   #
}}, {{
  0xe, // ###
  0xa, // # #
  0xe, // ###
  0xa, // # #
  0xe, // ###
}}, {{
  0xe, // ###
  0xa, // # #
  0xe, // ###
  0x2, //   #
  0x2, //   #
}},{{
  0x0, //
  0x4, //  #
  0x0, //
  0x4, //  #
  0x0, //
}},
  // ###
  // # #
  // ###
  // # #
  // # #
  //
  // ###
  // # #
  // ##
  // # #
  // ###
  //
  // ###
  // #
  // #
  // #
  // ###
  //
  // ##
  // # #
  // # #
  // # #
  // ##
  //
  // ###
  // #
  // ##
  // #
  // ###
  //
  // ###
  // #
  // ##
  // #
  // #
  //
  // ###
  // #
  // # #
  // # #
  // ###
  //
  // # #
  // # #
  // ###
  // # #
  // # #
  //
  // ###
  //  #
  //  #
  //  #
  // ###
  //
  //   #
  //   #
  //   #
  // # #
  //  #
  //
  // # #
  // # #
  // ##
  // # #
  // # #
  //
  // #
  // #
  // #
  // #
  // ###
  //
  // # #
  // ###
  // # #
  // # #
  // # #
  //
  //
  // #
  // ###
  // # #
  // # #
  //
  // ###
  // # #
  // # #
  // # #
  // ###
  //
  // ###
  // # #
  // ###
  // #
  // #
  //
  // ###
  // # #
  // # #
  // ###
  //   #
  //
  // ###
  // # #
  // ###
  // ##
  // # #
  //
  // ###
  // #
  // ###
  //   #
  // ###
  //
  // ###
  //  #
  //  #
  //  #
  //  #
  //
  // # #
  // # #
  // # #
  // # #
  // ###
  //
  // # #
  // # #
  // # #
  // # #
  //  #
  //
  // # #
  // # #
  // # #
  // ###
  // # #
  //
  // # #
  // # #
  //  #
  // # #
  // # #
  //
  // # #
  // # #
  //  #
  //  #
  //  #
  //
  // ###
  //   #
  //  #
  // #
  // ###
};


#define XSIZE 32
#define YSIZE 8

int put_digit(int c, int x, int y, int scroll_buffer[XSIZE][YSIZE]) {

    int h,w;

    for(h=0;h<DIGIT_HEIGHT;h++) {
       for(w=0;w<DIGIT_WIDTH;w++) {
          if (digits[c].line[h] & (1<<(DIGIT_WIDTH-1-w))) {
             scroll_buffer[x+w][y+h]=1;
          }
          else {
             scroll_buffer[x+w][y+h]=0;
          }
       }
    }

    return 0;
}

int main(int argc, char **argv) {

	int result,x,y;
	int x_scroll=0,scroll_dir=1;

	int scroll_buffer[XSIZE][YSIZE];
	unsigned short display_buffer[8];

	int i2c_fd;

	time_t seconds;
	struct tm *breakdown;

	/* init scroll buffer */
	for(x=0;x<XSIZE;x++) {
		for(y=0;y<YSIZE;y++) {
			scroll_buffer[x][y]=0;
		}
	}

	/* Open device */
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


	/* Scroll */

	while(1) {

		seconds=time(NULL);
		breakdown=localtime(&seconds);

		/* hour */
		put_digit((breakdown->tm_hour / 10),6,1,scroll_buffer);
		put_digit((breakdown->tm_hour % 10),10,1,scroll_buffer);

		/* colon */
		put_digit(10,14,1,scroll_buffer);

		/* minutes */
		put_digit((breakdown->tm_min / 10),18,1,scroll_buffer);
		put_digit((breakdown->tm_min % 10),24,1,scroll_buffer);

		/* Put scroll buffer into output buffer */
		for(y=0;y<YSIZE;y++) {
			/* clear the line before drawing to it */
			display_buffer[y]=0;
			for(x=0;x<XSIZE;x++) {
				if (scroll_buffer[x+x_scroll][y]) {
					plotxy_16(display_buffer,x,y);
				}
			}
		}

		update_8x16_display(i2c_fd,HT16K33_ADDRESS0,
			display_buffer,0,BROKEN);

		x_scroll+=scroll_dir;
		if ((x_scroll>13) || (x_scroll<1)) {
			scroll_dir=-scroll_dir;
		}

		usleep(100000);
	}

	return result;
}

