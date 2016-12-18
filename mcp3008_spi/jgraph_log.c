#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#include <sys/time.h>


/* So the way the Pi cluster works */
/* Each 5V power rail has a 0.1 Ohm resistor across it */
/* A differential amplifier (MCP6004) amplifies the voltage drop by 20 */
/* This is fed into the even channels of the MCP3008 */
/* 5V is fed into the odd channels */

/* WHy?  possibly I meant to make it possible to have an external */
/* Reference 5v, but in the end all the 5V lines are tied together */

int main(int argc, char **argv) {

	int in_fd,i,j;
	int result;

	char cbuffer[100];
	unsigned short sbuffer[6];

	struct timeval start_time;
	int seconds,usecs;

	if (argc<1) {
		printf("Usage %s logfile\n",argv[0]);
		exit(-1);
	}
for(i=0;i<4;i++) {

	printf("(* Node %d *)\n",i);

	in_fd=open(argv[1],O_RDONLY);
	if (in_fd<0) {
		printf("error opening, %s\n",strerror(errno));
		exit(-1);
	}

	read(in_fd,&cbuffer,4);

	if (
		(cbuffer[0]=='V') &&
		(cbuffer[1]=='M') &&
		(cbuffer[2]=='W') &&
		(cbuffer[3]=='*')) {
	}
	else {
		printf("wrong file type\n");
		exit(-1);
	}

	read(in_fd,&(start_time.tv_sec),sizeof(start_time.tv_sec));
	read(in_fd,&(start_time.tv_usec),sizeof(start_time.tv_usec));

	seconds=start_time.tv_sec;
	usecs=start_time.tv_usec;

	while(1) {
		result=read(in_fd,&sbuffer,6*sizeof(short));
		if (result<1) break;

		int total_diff;
		total_diff=(sbuffer[0]<<16)|sbuffer[1];

		usecs+=total_diff;
		while(usecs>1000000) {
			seconds++;
			usecs-=1000000;
		}

		printf("%d.%06d\t",seconds,usecs);

		for(j=0;j<4;j++) {

			double ref;
			double voltage,deltav,current,power;

			ref=4.90;
			voltage=((double)sbuffer[2+j])/1024.0;
			voltage*=ref;
			deltav=voltage/20.0;
			current=deltav/0.1;
			power=ref*current;
			if (j==i) printf("\t%.2f",power);

		}

		printf("\n");

	}

	close(in_fd);
}

	return 0;
}
