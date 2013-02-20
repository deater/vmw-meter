#include <stdio.h>
#include <stdlib.h>    /* exit()    */
#include <string.h>    /* strncmp() */
#include <unistd.h>    /* sleep()   */
#include <fcntl.h>     /* open()    */
#include <sys/ioctl.h> /* ioctl() */

#include "ppdev.h"

/* We get cpu usage by reading /proc/stat on Linux */
/* the cpu line totals all of the values, cpu0-cpuX are individual cpu stats */
/*
    - user: normal processes executing in user mode
    - nice: niced processes executing in user mode
    - system: processes executing in kernel mode
    - idle: twiddling thumbs
    - iowait: waiting for I/O to complete
    - irq: servicing interrupts
    - softirq: servicing softirqs
  
  We read the idle field and 100-idle = cpu usage
  
*/

   static int parport;

#define BARGRAPH 0
#define CYLON    1
#define RANDOM   2

#define SCREEN  0
#define PARPORT 1

#define UPDATES 50


unsigned char bargraph_lut[8]= { 0x0,0x80,0xc0,0xe0,0xf8,0xfc,0xfe,0xff};
unsigned char cylon_lut[8]= {0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01};
unsigned big_cylon_lut[14]={0,1,2,3,4,5,6,7,6,5,4,3,2,1};



static unsigned char bargraph(int percent) {
   
   int temp;
   
   
      /* 25 = USER_HZ /4 ... 7 = number of values */
   temp = percent/(25/7);
   if (temp>7) temp=7;
      
   return bargraph_lut[temp];
}

static unsigned char cylon(int percent) {
   
   static int cylon_count=0;
   
   cylon_count+=3+((percent*97)/100);
   
   if (cylon_count>=1400) cylon_count=0;
   
   return cylon_lut[big_cylon_lut[cylon_count/100]];
}

static unsigned char rando(int percent) {
   
   static int random_count=0;
   static char pattern=0;
   
   random_count+=4+((percent*96)/100);
   
   if (random_count>100) {
      random_count=0;
      pattern=rand()&0xff;
   }
   
   return pattern;
	
}

   
   


static void screen_output(unsigned char bitmap) {

   int i;

     /* clear scren ANSI code */
   printf("%c[H%c[2J",27,27);
   
   for(i=7;i>=0;i--) {
      if (bitmap & (1<<i)) printf("*"); else printf(" ");
   }
   printf("\n");

}

static void parport_output(unsigned char bitmap) {

   ioctl(parport,PPWDATA,&bitmap);
      
}

   
   
#define POLLING 25

int main(int argc, char **argv) {

   
   FILE *stat;
   long idle_cpu,old_idle,num_cpus=-1,idle_total,percent=0;
   long pattern_type=CYLON,output_type=PARPORT;
   char temp_string[BUFSIZ],pattern=0;
   int polling=0;


      /*******************************/
      /* scan command line arguments */
      /*******************************/
   /* TODO */
   
      /************************************/
      /* get initial info from /proc/stat */
      /************************************/
   
   stat=fopen("/proc/stat","r");
   if (stat==NULL) {
      printf("Error opening stats file!\n");
      exit(1);
   }

   while(1) {
      fgets(temp_string,BUFSIZ,stat);

         /* Count number of cpus */
         /* start at -1 because always have a total "cpu" line */
      if (!strncmp(temp_string,"cpu",3)) {
	 if (num_cpus<0) {
	    sscanf(temp_string,"%*s %*i %*i %*i %li",&old_idle);
	 }
	 num_cpus++;
      }

      if (feof(stat)) break;
   }
   
   fclose(stat);
   

      /* This depends on USER_HZ == 100 */
   idle_total=(100/(UPDATES/POLLING))*num_cpus;


      /********************************/
      /* Parallel Port Initialization */
      /********************************/
   if (output_type==PARPORT) {
      parport=open("/dev/parport0",O_RDWR);
      if (parport<0) {
	 printf("Error opening parallel port!\n");
	 exit(1);
      }
      ioctl(parport,PPCLAIM,NULL);
   }
   
	
   
      /********************/
      /* Main Loop        */
      /********************/
   
   while(1) {
      
      polling++;
      
      if (polling==POLLING) {
	 polling=0;
	   
         stat=fopen("/proc/stat","r");
         fscanf(stat,"%*s %*i %*i %*i %li",&idle_cpu);
         fclose(stat);
      
         percent=(idle_total-(idle_cpu-old_idle))*(UPDATES/POLLING);
	 if (percent<0) percent=0;
	 if (percent>100) percent=100;
         old_idle=idle_cpu;
      }
      
//      printf("Percent=%i\n",percent);
      
      switch(pattern_type) {
       case BARGRAPH: pattern=bargraph(percent); break;
       case CYLON:    pattern=cylon(percent); break;
       case RANDOM:   pattern=rando(percent); break;
       default: printf("Error!\n");
      }

      switch(output_type) {
       case PARPORT: parport_output(pattern); break;
       case SCREEN: screen_output(pattern); break;
       default: printf("ERROR!\n");
      }
      
	   

      usleep(1000000/UPDATES);
   }

   return 0;
}


   
