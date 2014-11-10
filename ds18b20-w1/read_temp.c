#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#define SENSOR_T1 "/sys/bus/w1/devices/28-000005aaf7ed/w1_slave"

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

	double temp1;

	while(1) {

		temp1=read_temp(SENSOR_T1);
		printf("%.2lfC, %.2lfF\n",temp1,temp1*(9.0/5.0)+32.0);
		sleep(1);
	}

	return 0;
}
