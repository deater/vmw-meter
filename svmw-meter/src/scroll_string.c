/* scroll a string on the display */
/* cat README | ./scroll_string.ascii */
/* will display text file README to the display */

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "meter.h"
#include "meter_tools.h"

int main(int argc, char **argv) {

  int i,len=0,pos=0;
  unsigned short display_state[8];
  char string[256];

  init_meter();
   
  reset_display();
  display_config_sane();
  for(i=0;i<8;i++) { display_state[i]=0; }
  update_display(display_state);   
  
  while(1) {
    if (fgets(string,256,stdin)==NULL) break;
    
        len=strlen(string);
        pos=0;
    
        while(1) {
    

           for(i=0;i<6;i++) {
           if (pos+i<len-1) {
	      display_state[i]=ascii_lookup[(unsigned char)string[pos+i]];
           }
           else display_state[i]=0;
       }


       update_display(display_state);   
       /* pause longer if first char */
       if (pos==0) usleep(500000);
       usleep(500000);

       pos++;
       if (pos==len) break;
    }

    
  }

  return 0;
}


