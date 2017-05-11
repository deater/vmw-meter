#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static double c_to_f(double temp) {

	return ((temp*9.0)/5.0)+32.0;

}

int main(int argc, char **argv) {

	///sys/bus/w1/devices/28-000005ab5eaf/w1_slave
/*
91 01 4b 46 7f ff 0f 10 25 : crc=25 YES
91 01 4b 46 7f ff 0f 10 25 t=25062
*/


	FILE *fff;
	char string[BUFSIZ];
	char *ptr;
	double deg_c;

	fff=fopen("/sys/bus/w1/devices/28-000005ab5eaf/w1_slave","r");
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

	printf("W1: %.1lf%cC %.0lf%cF\n",deg_c,0xb0,c_to_f(deg_c),0xb0);


	fff=fopen("/sys/class/thermal/thermal_zone0/temp","r");
	if (fff==NULL) {
		fprintf(stderr,"Error, cannot open pi\n");
		return -1;
	}

	fgets(string,BUFSIZ,fff);

	deg_c=atof(string)/1000.0;

	printf("P1: %.1lf%cC %.0lf%cF\n",deg_c,0xb0,c_to_f(deg_c),0xb0);

	fclose(fff);

	return 0;
}
