#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>

#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/resource.h>

#include "display.h"

#include "version.h"

int display_type=DISPLAY_I2C;

static double c_to_f(double temp) {

	return ((temp*9.0)/5.0)+32.0;

}

static double get_temp_w1(void) {

	///sys/bus/w1/devices/28-000005ab5eaf/w1_slave
/*
91 01 4b 46 7f ff 0f 10 25 : crc=25 YES
91 01 4b 46 7f ff 0f 10 25 t=25062
*/


	FILE *fff;
	DIR *sysdir;

	struct dirent *entry;

	char string[BUFSIZ],filename[BUFSIZ];
	char *ptr;
	double deg_c;

	sysdir=opendir("/sys/bus/w1/devices/");
	if (sysdir==NULL) {
		fprintf(stderr,"couldn't open sys dir\n");
		return -1000.0;
	}

	while(1) {
		entry=readdir(sysdir);
		if (entry==NULL) break;
//		printf("%s %d\n",entry->d_name,strlen(entry->d_name));
		if ((strlen(entry->d_name)==15)&&(entry->d_name[2]=='-')) {
//			printf("%s\n",entry->d_name);
			break;
		}
	}

	closedir(sysdir);

	if (entry==NULL) {
		fprintf(stderr,"temp not found\n");
		return -1000.0;
	}

	sprintf(filename,"/sys/bus/w1/devices/%s/w1_slave",entry->d_name);

	fff=fopen(filename,"r");
	if (fff==NULL) {
		fprintf(stderr,"temp not found\n");
		return -1;
	}

	fgets(string,BUFSIZ,fff);
	fgets(string,BUFSIZ,fff);

	ptr=strstr(string,"t=");
	ptr+=2;

	deg_c=atof(ptr)/1000.0;


	fclose(fff);

	return deg_c;
}


static double get_temp_pi(void) {

	FILE *fff;
	char string[BUFSIZ];
	double deg_c;

	fff=fopen("/sys/class/thermal/thermal_zone0/temp","r");
	if (fff==NULL) {
		fprintf(stderr,"Error, cannot open pi\n");
		return -1000.0;
	}

	fgets(string,BUFSIZ,fff);

	deg_c=atof(string)/1000.0;

	fclose(fff);

	return deg_c;
}

static int display_temp(void) {

	char text[13];
        int i;
	double w1_temp,pi_temp;

        for(i=0;i<13;i++) text[i]=0;

	w1_temp=get_temp_w1();
	pi_temp=get_temp_pi();

	/* 0123456789AB */
	/* W  79^F 79^C */

	if (display_type&DISPLAY_I2C) {
		sprintf(text,"TEMPERATURE");
		display_string(DISPLAY_I2C,text);
		sleep(2);

		sprintf(text,"W1%3.0lf%cF %2.0lf%cC",
			c_to_f(w1_temp),0xb0,w1_temp,0xb0);
		display_string(DISPLAY_I2C,text);
		sleep(2);

		sprintf(text,"PI%3.0lf%cF %2.0lf%cC",
			c_to_f(pi_temp),0xb0,pi_temp,0xb0);
		display_string(DISPLAY_I2C,text);
		sleep(2);

	}
	else {
		printf("Temperature\n");
		sleep(1);
		printf("W1: %.1lf%cC %.0lf%cF\n",w1_temp,0xb0,c_to_f(w1_temp),0xb0);
		sleep(1);
		printf("Pi: %.1lf%cC %.0lf%cF\n",pi_temp,0xb0,c_to_f(pi_temp),0xb0);
		sleep(1);
	}

	return 0;

}

static void quiet_and_exit(int sig) {

	if (display_type&DISPLAY_I2C) display_shutdown(DISPLAY_I2C);

	printf("Quieting and exiting\n");
	_exit(0);

}

static void print_help(int just_version, char *exec_name) {

	printf("\ntemperature version %s by Vince Weaver <vince@deater.net>\n\n",VERSION);
	if (just_version) exit(0);

	exit(0);
}

int main(int argc, char **argv) {

	int c;
	int result;


	/* Setup control-C handler to quiet the music	*/
	/* otherwise if you force quit it keeps playing	*/
	/* the last tones */
	signal(SIGINT, quiet_and_exit);

	/* Set to have highest possible priority */
	setpriority(PRIO_PROCESS, 0, -20);

	/* Parse command line arguments */
	while ((c = getopt(argc, argv, "dmhvmsnitr"))!=-1) {
		switch (c) {
			case 'h':
				/* help */
				print_help(0,argv[0]);
				break;
			case 'v':
				/* version */
				print_help(1,argv[0]);
				break;
			default:
				print_help(0,argv[0]);
				break;
		}
	}

	result=display_init(DISPLAY_I2C);
	if (result<0) {
		display_type=DISPLAY_TEXT;
	}

	while(1) {

		display_temp();
		sleep(5);

	}

	/* Quiet down the chips */
	display_shutdown(display_type);

	return 0;
}
