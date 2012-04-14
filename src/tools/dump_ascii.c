#include <stdio.h>
#include <unistd.h>

int main(int argc, char **argv) {
   int i=0;
   
   while(1) {
      
      printf("%c%c%c%c%c%c\n",
	     i,i+1,i+2,i+3,i+4,i+5);
     
      fflush(stdout);
      sleep(2);
      
      i+=6;
      if (i>255) break;
   }
   
   return 0;
}
