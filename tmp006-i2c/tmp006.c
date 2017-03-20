/* Makes a sine wave */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <math.h>

#include <sys/time.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

#include "i2c_lib.h"

static double c_to_f(double c) {

	return ((9.0/5.0)*c)+32;

}

static double calculate_temp(double Vobj,double Tdie) {

	double S,
		S0=5e-14, /* Calibration factor? TODO */
		a1=1.75e-3,
		a2=-1.678e-5,
		Tref=298.15,	/* Kelvin */
		Vos,
		b0=-2.94e-5,
		b1=-5.7e-7,
		b2=4.63e-9,
		fVobj,
		c2=13.4,
		Tobj;

	/* From Section 5.1 of the TMP006 User Guide */

	S=S0*(1.0+a1*(Tdie-Tref)+a2*(Tdie-Tref)*(Tdie-Tref));
	Vos=b0+b1*(Tdie-Tref)+b2*(Tdie-Tref)*(Tdie-Tref);
	fVobj=(Vobj-Vos)+c2*(Vobj-Vos)*(Vobj-Vos);

	Tobj=pow(((Tdie*Tdie*Tdie*Tdie)+(fVobj/S)),0.25);

	return Tobj;
}


int main(int argc, char **argv) {

	signed int result,temp;
	double temperature;

	int i2c_fd;
	unsigned char buffer[16];
	double vobject,tambient;

	struct timeval start_time, now_time;

	gettimeofday(&start_time, NULL);

	i2c_fd=init_i2c("/dev/i2c-1");
	if (i2c_fd < 0) {
		fprintf(stderr,"Error opening device!\n");
		return -1;
	}

#define TMP006_ADDR	0x40

#define TMP006_VOBJ_ADDR	0x0
#define TMP006_TAMBIENT_ADDR	0x1
#define TMP006_CONFIG		0x2
#define TMP006_MANID		0xfe
#define TMP006_DEVID		0xff

	/* Init TMP006 */
	if (ioctl(i2c_fd, I2C_SLAVE, TMP006_ADDR) < 0) {
		fprintf(stderr,"Error setting i2c address %x\n",
			TMP006_ADDR);
		return -1;
	}

        /* Point to config register */
	/* Start measurements */
	buffer[0]=TMP006_CONFIG;
	buffer[1]=0x74;
	buffer[2]=0x00;	/* active, default conversion rate, drdy disabled */
        if ( (write(i2c_fd, &buffer, 3)) !=3) {
                fprintf(stderr,"Error setting config addr!\n");
                return -1;
        }

	while(1) {
		/* Read Vobject */
		buffer[0]=TMP006_VOBJ_ADDR;
	        if ( (write(i2c_fd, &buffer, 1)) !=1) {
        	        fprintf(stderr,"Error setting vobj addr!\n");
                	return -1;
        	}

	        if ( (read(i2c_fd, &buffer, 2)) !=2) {
        	        fprintf(stderr,"Error reading vobj!\n");
                	return -1;
        	}

		temp=((buffer[0]<<8) | buffer[1]);
		if (temp&0x8000) temp|=0xffff0000;

		vobject=temp*156.25e-9; /* 156.25nV */
//		printf("Inout: %x Vobject=%.2lf V\n",
//			temp,vobject);

		/* Read Tambient */
		buffer[0]=TMP006_TAMBIENT_ADDR;
	        if ( (write(i2c_fd, &buffer, 1)) !=1) {
        	        fprintf(stderr,"Error setting vobj addr!\n");
                	return -1;
        	}

	        if ( (read(i2c_fd, buffer, 2)) !=2) {
        	        fprintf(stderr,"Error reading vobj!\n");
                	return -1;
        	}

		temp=((buffer[0]<<8) | buffer[1]);

		tambient=(temp>>2)/32.0;
//		printf("Inout: %x Tambient=%.2lf C\n",
//			temp,tambient);

		temperature=calculate_temp(
			vobject,
			tambient+272.15);

		gettimeofday(&now_time, NULL);
		printf("%d %.3lf (* C, %.3lfK %.3lfF *)\n",
			(int)(now_time.tv_sec-start_time.tv_sec),
			temperature-272.15,
			temperature,
			c_to_f(temperature-272.15));


//		printf("Temp=%.3lf K %.3lfC %.3lfF\n",
//			temperature,temperature-272.15,
//			c_to_f(temperature-272.15));

		sleep(1);
	}

	return result;
}

