/* Makes a spinning pattern on the display */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "i2c_lib.h"
#include "graphics_lib.h"


/* should make the device settable */
int ball_bounce_demo(void) {

   int i;
   double x,y,xadd,yadd;
   unsigned char display_buffer[DISPLAY_LINES];

   x=2.0, y=5.0;

   xadd=1.2; yadd=0.8;


   while(1) {
      for(i=0;i<DISPLAY_LINES;i++) display_buffer[i]=0;

      x+=xadd;
      if (x<0.0) {
         x=0.0;
         xadd=-xadd;
      }
      if (x>7.0) {
         x=7.0;
         xadd=-xadd;
      }

      y+=yadd;
      if (y<0.0) {
         y=0.0;
         yadd=-yadd;
      }
      if (y>7.0) {
         y=7.0;
         yadd=-yadd;
      }

      plotxy(display_buffer,(int)x,(int)y);
      update_display(display_buffer);
      usleep(100000);

   }

   return 0;
}


int main(int argc, char **argv) {

  int result;

  result=init_display();

  result=ball_bounce_demo();

  return result;
}

