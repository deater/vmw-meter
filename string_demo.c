#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "meter.h"
#include "meter_tools.h"

int main(int argc, char **argv) {

  int i;
  unsigned short display_state[8];
  char string[256];

  init_meter();
   
  reset_display();
  display_config_sane();
  for(i=0;i<8;i++) { display_state[i]=0; }
  update_display(display_state);   
  
  while(1) {
    if (fgets(string,256,stdin)==NULL) break;
    puts(string);

    for(i=0;i<6;i++) {
      if (i<strlen(string)-1) {
	display_state[i]=ascii_lookup[(unsigned char)string[i]];
      }
      else display_state[i]=0;
    }
    update_display(display_state);   
  }

  return 0;
}


