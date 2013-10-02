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

#define XSIZE 10
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


int flame_demo(void) {

   int x,y;
   unsigned char display_buffer[DISPLAY_LINES];

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
         for(x=1;x<9;x++) {
            if (b[x][y]>THRESH) {
               plotxy(display_buffer,x-1,y-1);
            }
         }
      }
      update_display(display_buffer);
      usleep(SLEEP);
   }

   return 0;
}


int main(int argc, char **argv) {

  int result;

  result=init_display("/dev/i2c-1",10);

  result=flame_demo();

  return result;
}

