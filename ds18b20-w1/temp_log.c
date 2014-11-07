#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#define SENSOR_T1 "/sys/bus/w1/devices/28-000005aa5fe4/w1_slave"
#define SENSOR_T2 "/sys/bus/w1/devices/28-000005aad011/w1_slave"
#define SENSOR_T3 "/sys/bus/w1/devices/28-000005aafab4/w1_slave"

/*
c2 01 4b 46 7f ff 0e 10 39 : crc=39 YES
c2 01 4b 46 7f ff 0e 10 39 t=28125
*/

static double read_temp(char *filename) {

	double result;
	int milic;
	char yesno[BUFSIZ],temp[BUFSIZ];
	FILE *fff;

	fff=fopen(filename,"r");
	if (fff==NULL) {
		printf("Error opening %s\n",SENSOR_T1);
		return -5000.0;
	}

	fscanf(fff,"%*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %s "
			"%*s %*s %*s %*s %*s %*s %*s %*s %*s %s",
			yesno,temp);

	fclose(fff);

	if (strncmp(yesno,"YES",3)) {
		printf("Invalid CRC!\n");
		return -1000.0;
	}
	milic=atoi(temp+2);

	result=milic/1000.0;

	return result;
}

int main(int argc, char **argv) {

	double temp1,temp2,temp3;
	struct timeval start_time, now_time;

	gettimeofday(&start_time, NULL);
	FILE *out1,*out2,*out3;

	out1=fopen("out.1","w");
	out2=fopen("out.2","w");
	out3=fopen("out.3","w");

	if (!out1 || !out2 || !out3) {
		printf("Error opening!\n");
		exit(1);
	}

	while(1) {
		gettimeofday(&now_time, NULL);
		temp1=read_temp(SENSOR_T1);
		fprintf(out1,"%d %.2lf\n",now_time.tv_sec-start_time.tv_sec,
			temp1);
		sleep(1);

		gettimeofday(&now_time, NULL);
		temp2=read_temp(SENSOR_T2);
		fprintf(out2,"%d %.2lf\n",now_time.tv_sec-start_time.tv_sec,
			temp2);
		sleep(1);

		gettimeofday(&now_time, NULL);
		temp3=read_temp(SENSOR_T3);
		fprintf(out3,"%d %.2lf\n",now_time.tv_sec-start_time.tv_sec,
			temp3);
		sleep(1);

//		printf("Temps: %.2lf %.2lf %.2lf\n",temp1,temp2,temp3);

		fflush(out1);
		fflush(out2);
		fflush(out3);
	}

	return 0;
}
