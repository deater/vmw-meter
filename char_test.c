#include <stdio.h>

#include "meter.h"
#include "meter_tools.h"

void print_char(short value) {

  if (value & SEGMENT_A) printf(" ___ \n");
  else printf("     \n");

  if (value & SEGMENT_F) printf("|");
  else printf(" ");
  if (value & SEGMENT_P) printf("\\");
  else printf(" ");
  if (value & SEGMENT_G) printf("|");
  else printf(" ");
  if (value & SEGMENT_H) printf("/");
  else printf(" ");
  if (value & SEGMENT_B) printf("|\n");
  else printf(" \n");

  if (value & SEGMENT_N) printf(" - ");
  else printf("   ");
  if (value & SEGMENT_J) printf("- \n");
  else printf("  \n");

  if (value & SEGMENT_E) printf("|");
  else printf(" ");
  if (value & SEGMENT_M) printf("/");
  else printf(" ");
  if (value & SEGMENT_L) printf("|");
  else printf(" ");
  if (value & SEGMENT_K) printf("\\");
  else printf(" ");
  if (value & SEGMENT_C) printf("|\n");
  else printf(" \n");
  
  if (value & SEGMENT_D) printf(" --- \n");
  else printf("     \n");

  /*
 ___
|\|/|
 - - 
|/|\|
 ---  

  */
  return;
}

int main(int argc, char **argv) {

  int i;

  for(i=0;i<256;i++) {
     printf("%x ",i);
     if ((i>31 ) && (i< 127)) printf("'%c'",i);
     printf("\n");
     print_char(ascii_lookup[i]);     
  }
  return 0;
}
