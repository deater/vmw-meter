/* Makes a spinning pattern on the display */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "i2c_lib.h"

/* should make the device settable */
int fill_demo(void) {

   int i,x,y;
   unsigned char display_buffer[DISPLAY_LINES];

   while(1) {
      for(i=0;i<DISPLAY_LINES;i++) display_buffer[i]=0;

      for(y=0;y<DISPLAY_LINES;y++) {
         for(x=0;x<DISPLAY_LINES;x++) {

            usleep(50000);

            /* write out to hardware */
            display_buffer[y]<<=1;
            display_buffer[y]+=1;
            update_display(display_buffer);
         }
      }
      usleep(500000);
   }

   return 0;
}


int main(int argc, char **argv) {

  int result;

  result=init_display("/dev/i2c-1",10);

  result=fill_demo();

  return result;
}

