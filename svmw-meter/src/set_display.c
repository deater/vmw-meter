/* This tool lets you set all of the segments on the display */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include "meter.h"
#include "meter_tools.h"

int main(int argc, char **argv) {

  int i;
  unsigned short display_state[8];
  char string[256],*endptr;
  int top,dp,left,right;
   
  if (argc<6) {
     printf("Usage: %s string top_leds decimal_points left_bar right_bar\n",
	    argv[0]);
     exit(1);
  }

  strcpy(string,argv[1]);
   
  init_meter();
   
  reset_display();
  display_config_sane();
   
  for(i=0;i<8;i++) { display_state[i]=0; }
  update_display(display_state);   
  
   /* set string */
  for(i=0;i<6;i++) {
     if (i<strlen(string)) {
	display_state[i]=ascii_lookup[(unsigned char)string[i]];
      }
      else display_state[i]=0;
  }
   
  /* top */
  top=strtol(argv[2], &endptr, 0);
  for(i=0;i<6;i++) {
     if (top&(1<<(5-i))) display_state[i]|=SEGMENT_EX;
  }
   
  /* dp */
  dp=strtol(argv[3], &endptr, 0);
  for(i=0;i<6;i++) {
     if (dp&(1<<(5-i))) display_state[i]|=SEGMENT_DP;
  }   
   
  /* left */
  left=strtol(argv[4], &endptr, 0);
  display_state[6]=left;
 
  /* right */
  right=strtol(argv[5], &endptr, 0);
  display_state[7]=right;

  /* update the display */
  update_display(display_state);   

  return 0;
}


