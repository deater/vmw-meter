#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <stdlib.h>

#include "14seg_font.h"
#include "i2c_lib.h"
#include "default_device.h"

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

unsigned short bargraph_lut[11]=
     {0x0000,
      0x8000,
      0xc000,
      0xe000,
      0xf000,
      0xf800,
      0xfc00,
      0xfe00,
      0xff00,
      0xff80,
      0xffc0
     };

unsigned short cylon_left[12]=
     {0x20,0x10,0x08,0x04,0x02,0x01, 0x00,0x00,0x00,0x00,0x00,0x00};
unsigned char cylon_right[12]=
     {0x00,0x00,0x00,0x00,0x00,0x00, 0x01,0x02,0x04,0x08,0x10,0x20};
unsigned big_cylon_lut[22]={0,1,2,3,4,5,6,7,8,9,10,11,10,9,8,7,
                            6,5,4,3,2,1};



unsigned short_cylon_lut[10]={
   0x01,
   0x02,
   0x04,
   0x08,
   0x10,
   0x20,
   0x10,
   0x08,
   0x04,
   0x02};


int main(int argc, char **argv) {

	int meter_fd,display_present;
	int i,random_top_cylon_bottom=0;
	unsigned short display_state[8];
	char string[256];
	int cylon_count=0;

	FILE *stat;
	long idle_cpu,old_idle,num_cpus=-1;
	long even_cpus,odd_cpus,percent=0,current_percent;
	long temp_idle,odd_idle,even_idle,odd_percent,even_percent;
	long old_odd=0,old_even=0;
	int idle_ticks,odd_ticks,even_ticks;
	char temp_string[BUFSIZ];
	int random_count=0;
	int pattern=0,rand_pattern=0;

	int last_cpu[5]={0,0,0,0,0};
	int last_even[5]={0,0,0,0,0};
	int last_odd[5]={0,0,0,0,0}; 

	int last_pointer=0;

	/************************************/
	/* get initial info from /proc/stat */
	/************************************/
	stat=fopen("/proc/stat","r");
	if (stat==NULL) {
		printf("Error opening stats file!\n");
		exit(1);
	}

	/* Count number of cpus */
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

	fclose(stat);			/*  0  1  2  3  4  5  6  7 */
	even_cpus=(num_cpus+1)/2;	/*  0  1  1  2  2  3  3  4 */
	odd_cpus=(num_cpus/2);		/*  0  0  1  1  2  2  3  3 */


	/* Init Display */
	display_present=1;
	meter_fd=init_i2c(DEFAULT_DEVICE);
	if (meter_fd < 0) {
		fprintf(stderr,"Error opening device %s!\n",DEFAULT_DEVICE);
		display_present=0;
	}

	if (display_present) {
		init_saa1064_display(meter_fd,SAA1064_ADDRESS1);
		init_saa1064_display(meter_fd,SAA1064_ADDRESS2);
		init_saa1064_display(meter_fd,SAA1064_ADDRESS3);
		init_saa1064_display(meter_fd,SAA1064_ADDRESS4);
	}

	for(i=0;i<8;i++) display_state[i]=0;

	if (display_present) {
		update_saa1064_display(meter_fd, SAA1064_ADDRESS1,
			display_state);
	}
	else {
		update_saa1064_ascii(display_state);
	}

	while(1) {

		stat=fopen("/proc/stat","r");
		fgets(temp_string,BUFSIZ,stat);
		sscanf(temp_string,"%*s %*i %*i %*i %li",&idle_cpu);

		odd_idle=0;
		even_idle=0;
		for(i=0;i<num_cpus;i++) {
			fgets(temp_string,BUFSIZ,stat);
			sscanf(temp_string,"%*s %*i %*i %*i %li",&temp_idle);

			if (i&1) odd_idle+=temp_idle;
			else even_idle+=temp_idle;
		}
		fclose(stat);

		idle_ticks=idle_cpu-old_idle;
		odd_ticks=odd_idle-old_odd;
		even_ticks=even_idle-old_even;

		/* we check 5 times a second (5Hz) */
		/* stats are updated 100 times a second (100Hz) */

		/* completely idle = 20*cpus */
		/* completely busy =  0 */
		/* so percent busy = 100 - 5*(idle/cpus) */

		current_percent=100-(5*(idle_ticks/num_cpus));
		if (current_percent<0) current_percent=0;
		if (current_percent>100) current_percent=100;

		odd_percent=100-(5*(odd_ticks/odd_cpus));
		if (odd_percent<0) odd_percent=0;
		if (odd_percent>100) odd_percent=100;

		even_percent=100-(5*(even_ticks/even_cpus));
		if (even_percent<0) even_percent=0;
		if (even_percent>100) even_percent=100;

		last_cpu[last_pointer]=current_percent;
		last_odd[last_pointer]=odd_percent;
		last_even[last_pointer]=even_percent;

		last_pointer++; if (last_pointer>4) last_pointer=0;

		old_idle=idle_cpu;
		old_odd=odd_idle;
		old_even=even_idle;

		percent=0;
		odd_percent=0;
		even_percent=0;
		for(i=0;i<5;i++) {
			percent+=last_cpu[i];
			odd_percent+=last_odd[i];
			even_percent+=last_even[i];
		}
		percent/=5;
		odd_percent/=5;
		even_percent/=5;

		if (num_cpus==1) {
			odd_percent=percent;
			even_percent=percent;
 		}

		/* text area */

		sprintf(string,"CPU%3ld",percent);
		display_state[0]=ascii_lookup[(int)string[0]];
		display_state[1]=ascii_lookup[(int)string[1]];
		display_state[2]=ascii_lookup[(int)string[2]];
		display_state[3]=ascii_lookup[(int)string[3]];
		display_state[4]=ascii_lookup[(int)string[4]];
		display_state[5]=ascii_lookup[(int)string[5]];


		/* bargraph */

		display_state[6]=bargraph_lut[odd_percent/10];
		display_state[7]=bargraph_lut[even_percent/10];


		if (random_top_cylon_bottom) {

		/* cylon */
		cylon_count+=3+((percent*97)/50);

		if (cylon_count>=2200) cylon_count=0;

			display_state[6]|=cylon_left[big_cylon_lut[cylon_count/100]];
			display_state[7]|=cylon_right[big_cylon_lut[cylon_count/100]];

			/* random */
			random_count+=4+((percent*96)/100);

			if (random_count>75) {
				random_count=0;
				pattern=rand()&0x3f;
			}
			for(i=0;i<6;i++) {
				if ((pattern>>i)&0x1) display_state[i]^=SAA1064_SEGMENT_EX;
			}
		}
		else {
			/* cylon */
			cylon_count+=3+((percent*97)/50);

			if (cylon_count>=1000) cylon_count=0;

			pattern=short_cylon_lut[cylon_count/100];

			for(i=0;i<6;i++) {
				if ((pattern>>i)&0x1) display_state[i]^=SAA1064_SEGMENT_EX;
			}

			/* random */
			random_count+=4+((percent*96)/100);

			if (random_count>75) {
				random_count=0;
				rand_pattern=rand()&0x0fff;
			}

			display_state[6]|=rand_pattern&0x3f;
			display_state[7]|=(rand_pattern>>6)&0x3f;

		}


		if (display_present) {
			update_saa1064_display(meter_fd, SAA1064_ADDRESS1,
				display_state);
		}
		else {
			update_saa1064_ascii(display_state);
		}

		/* 200,000 = 5Hz*/
		usleep(200000);
	}


	return 0;
}


