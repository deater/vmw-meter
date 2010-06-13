#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "meter.h"
#include "meter_tools.h"

int main(int argc, char **argv) {

  int i,spin=0,dot1=0,dot2=19,blob=0;
  unsigned short display_state[8];

  init_meter();
  reset_display();
  display_config_sane();
  
  while(1) {
     for(i=0;i<6;i++) {
       display_state[i]=0xfc00|(0x0200>>spin);              
       //       printf("%d %d %x %x\n",i,spin,0x2000,display_state[i]);
     }
     spin++;
     if (spin>7) spin=0;

     display_state[6]=0;
     display_state[7]=0;

     if (dot1<16) display_state[6]|=1<<dot1;
     else if (dot1<22) display_state[dot1-16]|=SEGMENT_DP;
     else display_state[7]=1<<(15-(dot1-22));

     if (dot2<16) display_state[6]|=1<<dot2;
     else if (dot2<22) display_state[dot2-16]|=SEGMENT_DP;
     else display_state[7]=1<<(15-(dot2-22));

     dot1++;
     dot2++;
     if (dot1>37) dot1=0;
     if (dot2>37) dot2=0;

     if (spin%2) blob=rand();
     for(i=0;i<6;i++) {
       if (blob&1<<i) display_state[i]|=SEGMENT_EX;
     }
 
     update_display(display_state);   
     usleep(100000);
  }


  return 0;
}


