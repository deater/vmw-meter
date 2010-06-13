#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "meter.h"
#include "meter_tools.h"

int main(int argc, char **argv) {

  int i,pulse1=0,pulsedir1=1,pulse2=15,pulsedir2=-1,count=0;
  unsigned short display_state[8];

  init_meter();
  reset_display();
  display_config_sane();
  
  while(1) {
     for(i=0;i<6;i++) {
       if (count&0x4) display_state[i]=0xaaaa;
       else display_state[i]=0x5555;
     }
     display_state[6]=~((1<<pulse1)-1);
     display_state[7]=~((1<<pulse2)-1);
     update_display(display_state);   
     usleep(100000);

     count++;

     pulse1=pulse1+pulsedir1;
     if ((pulse1>15) || (pulse1<1)) pulsedir1=-pulsedir1;
     pulse2=pulse2+pulsedir2;
     if ((pulse2>15) || (pulse2<1)) pulsedir2=-pulsedir2;
     
  }


  return 0;
}


