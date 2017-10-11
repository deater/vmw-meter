/* TODO:
	Skip through entries with < and >
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>

#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>

#include "stats.h"
#include "display.h"

#include "lib_lib.h"

#define HISTORY_SIZE 16

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

struct temp_type {
	double pi;
	double w1;
};

static int display_temp(struct temp_type *t, int which) {

	char text[13];
        int i;

        for(i=0;i<13;i++) text[i]=0;

	/* 0123456789AB */
	/* W  79^F 79^C */

	if (display_type&DISPLAY_I2C) {
		if (which==0) {
			sprintf(text,"TEMPERATURE");
		}
		else if (which==1) {
			sprintf(text,"W1%3.0lf%cF %2.0lf%cC",
				c_to_f(t->w1),0xb0,t->w1,0xb0);
		}
		else if (which==2) {
			sprintf(text,"PI%3.0lf%cF %2.0lf%cC",
				c_to_f(t->pi),0xb0,t->pi,0xb0);
		}
		display_14seg_string(DISPLAY_I2C,text);
	}
	else {
		if (which==0) {
			printf("Temperature\n");
		}
		else if (which==1) {
			printf("W1: %.1lf%cC %.0lf%cF\n",
				t->w1,0xb0,c_to_f(t->w1),0xb0);
		}
		else if (which==2) {
			printf("Pi: %.1lf%cC %.0lf%cF\n",
				t->pi,0xb0,c_to_f(t->pi),0xb0);
		}
	}

	return 0;

}



int lib_temperature(void) {

	int i,state=0;
	int current_pointer=0;
	int count=0;
	unsigned char buffer[16];
	int which,ch;
	double pif,wif;

	struct temp_type history[HISTORY_SIZE];
	struct temp_type current;

	double w1_temp,pi_temp;

	/* Make invalid entries invalid */
	for(i=0;i<HISTORY_SIZE;i++) {
		history[i].pi=-500.0;
		history[i].w1=-500.0;
	}

	while(1) {

		/* This should take roughly 15 seconds */

		state=0;
		display_temp(&current,state);

		w1_temp=get_temp_w1();
		pi_temp=get_temp_pi();

		current.pi=pi_temp;
		current.w1=w1_temp;
		ch=sleep_unless_keypress(3);
		if ((ch==CMD_EXIT_PROGRAM) || (ch==CMD_CANCEL)) {
			break;
		}

		state++;
		display_temp(&current,state);
		ch=sleep_unless_keypress(3);
		if ((ch==CMD_EXIT_PROGRAM) || (ch==CMD_CANCEL)) {
			break;
		}

		state++;
		display_temp(&current,state);

		ch=sleep_unless_keypress(3);
		if ((ch==CMD_EXIT_PROGRAM) || (ch==CMD_CANCEL)) {
			break;
		}

		state=0;

		ch=sleep_unless_keypress(6);
		if ((ch==CMD_EXIT_PROGRAM) || (ch==CMD_CANCEL)) {
			break;
		}

		count++;

		if (count==4) {
			count=0;
			history[current_pointer].w1=current.w1;
			history[current_pointer].pi=current.pi;

			/* circular list */
			current_pointer++;
			current_pointer=current_pointer%HISTORY_SIZE;

			/* plot the history */
			for(i=0;i<HISTORY_SIZE;i++) {

				which=(current_pointer+i)%HISTORY_SIZE;

				pif=c_to_f(history[which].pi);
				wif=c_to_f(history[which].w1);

//				if (history[which].pi>-400.0) {
//					printf("%d %lf %lf %d %d\n",
//						i,
//						c_to_f(history[which].pi),
//						c_to_f(history[which].w1),
//						(int)((pif-70.0)/5),
//						(int)((wif-70.0)/5)
//						);
//				}

				buffer[i]=0;

				if ((pif<70.0) || (pif>110)) {

				}
				else {
					buffer[i]|=(1<<(int)((pif-70)/5));
				}

				if ((wif<70.0) || (wif>110)) {

				}
				else {
					buffer[i]|=(1<<(int)((wif-70)/5));
				}
			}
			display_8x16_vertical(display_type,buffer);
		}
	}

	return 0;
}
