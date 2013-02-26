#include <stdio.h>

#include "i2c_lib.h"

int plotxy(unsigned char *display, int x, int y) {

   if ((y<0) || (y>=DISPLAY_LINES)) {
      fprintf(stderr,"Y %d out of bounds!\n",y);
      return -1;
   }

   if ((x<0) || (y>7)) {
      fprintf(stderr,"X %d out of bounds!\n",y);
      return -1;
   }

   display[y]|=(1<<(7-x));

   return 0;
}
