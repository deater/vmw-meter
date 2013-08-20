/* Makes a clock */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include "i2c_lib.h"

#if 0

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
#endif

int main(int argc, char **argv) {

 	int result;

 	unsigned short display_buffer[8];
	long long keypad_result=0,old_keypad=0,keypad_change;

	time_t seconds;
	struct tm *breakdown;

	/* brightness 0 - 15 */
	result=init_display(12);

	display_buffer[0]=0xffff;
	display_buffer[1]=0xffff;
	display_buffer[2]=0xffff;
	display_buffer[3]=0xffff;
	display_buffer[4]=0xffff;
	display_buffer[5]=0xffff;
	display_buffer[6]=0xffff;
	display_buffer[7]=0xffff;


	while(1) {

		seconds=time(NULL);
		breakdown=localtime(&seconds);

#if 0
     /* hour */
     put_digit((breakdown->tm_hour / 10),1,1,scroll_buffer);
     put_digit((breakdown->tm_hour % 10),5,1,scroll_buffer);

     /* colon */
     put_digit(10,9,1,scroll_buffer);

     /* minutes */
     put_digit((breakdown->tm_min / 10),13,1,scroll_buffer);
     put_digit((breakdown->tm_min % 10),17,1,scroll_buffer);

     /* Put scroll buffer into output buffer */
     for(y=0;y<YSIZE;y++) {
        /* clear the line before drawing to it */
        display_buffer[y]=0;
        for(x=0;x<XSIZE;x++) {
           if (scroll_buffer[x+x_scroll][y]) plotxy(display_buffer,x,y);
        }
     }
#endif


		keypad_result=read_keypad();
//		if (keypad_result!=-1) {
//			printf("keypad: %lld\n",keypad_result);
//		}

		keypad_change=old_keypad&~keypad_result;
		if (keypad_change) {
			printf("Keypad: %llx\n",keypad_change);
			display_buffer[0]^=keypad_change;
		}

		old_keypad=keypad_result;

        	update_display_raw(display_buffer);


		usleep(100000);
	}

	return result;
}

