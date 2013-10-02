/* Makes a spinning pattern on the display */

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

int put_letter(unsigned char letter) {

   int i;
   unsigned char display_buffer[8];


   for(i=0;i<8;i++) {
      display_buffer[i]=font[letter][i];
   }
   update_display(display_buffer);

   return 0;
}


int main(int argc, char **argv) {

  int result,i;

  result=init_display("/dev/i2c-1",10);

  result=load_font("tbfont.tb1");

  while(1) {
     for(i=32;i<127;i++) {
        result=put_letter(i);
	usleep(500000);
     }
  }

  return result;
}

