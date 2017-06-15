#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <stdlib.h>
#include <signal.h>

#include "stats.h"
#include "display.h"

static int display_type=DISPLAY_I2C;

static void quiet_and_exit(int sig) {

	if (display_type&DISPLAY_I2C) display_shutdown(DISPLAY_I2C);

	printf("Quieting and exiting\n");
	_exit(0);
}



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

/* we only really plan to run this on a pi2? */
#define MAX_CPUS	16

int main(int argc, char **argv) {

	int i,j;
	char string[256];

	FILE *stat;
	FILE *mem;
	long total_idle,old_total_idle,num_cpus=-1;
	long idle[MAX_CPUS],idle_ticks[MAX_CPUS];
	long current_percent;
	long temp_idle,percent[MAX_CPUS];
	long old[MAX_CPUS];
	int total_idle_ticks;
	char temp_string[BUFSIZ];
	long total_percent;

	int last_cpu[5]={0,0,0,0,0};
	int last[MAX_CPUS][5];

	int last_pointer=0;
	int result;
	int cylon, cylon_count=0,cylon_dir=1;
	long long mem_total;
	long long mem_free;

	unsigned char buffer[16];

	/* Init Variables */
	for(i=0;i<MAX_CPUS;i++) for(j=0;j<5;j++) last[i][j]=0;

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
				sscanf(temp_string,"%*s %*i %*i %*i %li",
					&old_total_idle);
			}
			num_cpus++;
		}

		if (feof(stat)) break;
	}

	fclose(stat);			/*  0  1  2  3  4  5  6  7 */

	printf("Found %ld cpus\n",num_cpus);
	if (num_cpus>MAX_CPUS) {
		printf("Truncating to %d\n",MAX_CPUS);
		num_cpus=MAX_CPUS;
	}

	/* Setup control-C handler to quiet the music   */
	/* otherwise if you force quit it keeps playing */
	/* the last tones */
	signal(SIGINT, quiet_and_exit);

	result=display_init(DISPLAY_I2C);
	if (result<0) {
		display_type=DISPLAY_TEXT;
	}

	while(1) {

		mem=fopen("/proc/meminfo","r");
		fscanf(mem,"%*s %lld %*s",&mem_total);
		fscanf(mem,"%*s %lld %*s",&mem_free);
		fclose(mem);

		stat=fopen("/proc/stat","r");
		fgets(temp_string,BUFSIZ,stat);
		sscanf(temp_string,"%*s %*i %*i %*i %li",&total_idle);

		for(i=0;i<num_cpus;i++) {
			fgets(temp_string,BUFSIZ,stat);
			sscanf(temp_string,"%*s %*i %*i %*i %li",&temp_idle);

			idle[i]=temp_idle;
		}
		fclose(stat);

		total_idle_ticks=total_idle-old_total_idle;

		for(i=0;i<num_cpus;i++) {
			idle_ticks[i]=idle[i]-old[i];
		}

		/* we check 5 times a second (5Hz) */
		/* stats are updated 100 times a second (100Hz) */

		/* completely idle = 20*cpus */
		/* completely busy =  0 */
		/* so percent busy = 100 - 5*(idle/cpus) */

		current_percent=100-(5*(total_idle_ticks/num_cpus));
		if (current_percent<0) current_percent=0;
		if (current_percent>100) current_percent=100;

		for(i=0;i<num_cpus;i++) {
			percent[i]=100-(5*(idle_ticks[i]));
			if (percent[i]<0) percent[i]=0;
			if (percent[i]>100) percent[i]=100;
		}

		last_cpu[last_pointer]=current_percent;
		for(i=0;i<num_cpus;i++) {
			last[i][last_pointer]=percent[i];
		}

		last_pointer++;
		if (last_pointer>4) last_pointer=0;

		old_total_idle=total_idle;
		for(i=0;i<num_cpus;i++) {
			old[i]=idle[i];
			percent[i]=0;
		}

		total_percent=0;
		for(i=0;i<5;i++) {
			total_percent+=last_cpu[i];
			for(j=0;j<num_cpus;j++) {
				percent[j]+=last[j][i];
			}
		}

		total_percent/=5;

		for(i=0;i<num_cpus;i++) {
			percent[i]/=5;
		}

		/* text area */

		sprintf(string,"CPU%3ldMEM%3lld",total_percent,
				100*(mem_total-mem_free)/mem_total);
		//printf("%s\n",string);
		display_14seg_string(display_type,string);

		/* bargraph on 8x16 */
//		for(i=0;i<num_cpus;i++) {
//			printf("CPU%d=%ld ",i,percent[i]);
//		}
//		printf("\n");

		for(i=0;i<16;i++) {
			buffer[i]=(1<<((percent[i/num_cpus]/12)))-1;
		}
		display_8x16_vertical(display_type,buffer);

		/* cylon bargraphs */

		cylon=(1<<(cylon_count/100));
		cylon_count+=(cylon_dir*(50+2*total_percent));

		if (cylon_count>999) {
			cylon_count=999;
			cylon_dir=-cylon_dir;
		}

		if (cylon_count<0) {
			cylon_count=0;
			cylon_dir=-cylon_dir;
		}

		bargraph_raw(display_type,
			cylon,0x0,0x0,
			cylon,0x0,0x0);

		/* 200,000 = 5Hz*/
		usleep(200000);
	}


	/* Quiet down the chips */
	display_shutdown(display_type);

	return 0;
}


