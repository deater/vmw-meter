/* Makes a sine wave */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <math.h>

#include "i2c_lib.h"
#include "graphics_lib.h"

#define XSIZE 8
#define YSIZE 8

#define PI 3.14159265358979323846264338327950

int sine_demo(void) {

   int x,y;
   double dx,temp_dx,dy,dxscroll=PI/8.0;

   int out_buffer[XSIZE][YSIZE];

   unsigned char display_buffer[DISPLAY_LINES];

   dx=0.0;

   while(1) {
      for(x=0;x<XSIZE;x++) for (y=0;y<YSIZE;y++) out_buffer[x][y]=0;

      for(x=0;x<XSIZE;x++) {
         temp_dx=dx+((double)x *(PI/8.0));
         dy=sin(temp_dx);
         y=(int)round((3.5+(dy*3.5)));
         out_buffer[x][y]=1;
//         printf("%d %d %lf %lf\n",x,y,temp_dx,dy);
      }

      /* Put scroll buffer into output buffer */
      for(y=0;y<YSIZE;y++) {
         /* clear the line before drawing to it */
         display_buffer[y]=0;
         for(x=0;x<XSIZE;x++) {
            if (out_buffer[x][y]) plotxy(display_buffer,x,y);
         }
      }

      update_display(display_buffer);

      dx+=dxscroll;
      usleep(50000);
   }

   return 0;
}


int main(int argc, char **argv) {

  int result;

  result=init_display(10);

  result=sine_demo();

  return result;
}

