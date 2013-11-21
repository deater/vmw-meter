/* Makes a clock */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include "i2c_lib.h"

unsigned short digits[16] = {
	0x003f,	// 0
	0x0006,	// 1
	0x005b,	// 2
	0x004f,	// 3
	0x0066,	// 4
	0x006d, // 5
	0x007d, // 6
	0x0007, // 7
	0x007f, // 8
	0x0067, // 9
	0x0077, // A
	0x007c, // B
	0x0039, // C
	0x005e, // D
	0x0079, // E
	0x0071, // F
};

double read_temp(void) {


	FILE *fff;
	int voltage;
	double deg_C,deg_F;

	/* This assumes a TMP36 temp probe hooked to ADCIN2 */
	/* on a gumstix board.                              */
	/*  degC = (100 * V) - 50                           */

	fff=fopen("/sys/class/hwmon/hwmon0/device/in2_input","r");
	if (fff==NULL) {
		printf("Error reading temp!\n");
		return -500.0;
	}

	/* read voltage in mV */
	fscanf(fff,"%d",&voltage);
	fclose(fff);

	deg_C=(100.0*((double)voltage)/1000)-50.0;

	deg_F= ((deg_C*9.0)/5.0)+32.0;

	return deg_F;
}


int main(int argc, char **argv) {

	unsigned short display_buffer[8];
	int i2c_fd,i,int_digit;
	double temp_f;

	/* init i2c */
	i2c_fd=init_i2c("/dev/i2c-3");
	if (i2c_fd < 0) {
		fprintf(stderr,"Error opening device!\n");
		return -1;
	}

	/* Init display */
        if (init_display(i2c_fd,HT16K33_ADDRESS1,10)) {
		fprintf(stderr,"Error opening display\n");
		return -1;
	}

/*
   --0A-
  |     |
  5F    1B       : = 1
  |     |
   -G6--
  |     |
  4E    2C
  |     |
   -3D--        . = 7DP


  8  8  :  8  8
  0  1  2  3  4  Column

 so to set segment G of the far left element, then 16 bit display_buffer

    display_buffer[0]=1<<6; // 0x40

 To put 0 in the far right display

	display_buffer[4]=0x003f;

*/
	while(1) {

		/* clear display */
		for(i=0;i<8;i++) display_buffer[i]=0x00;

		temp_f=read_temp();

		if (temp_f<-400.0) {
			/* error */
			display_buffer[0]=digits[0xE];	// E
			display_buffer[1]=0x50;		// r
			display_buffer[3]=0x50;		// r
		}
		else if (temp_f <0.0) {
			display_buffer[0]=0x40;		// -
			int_digit=(-temp_f);
			display_buffer[1]=digits[int_digit/10];
			display_buffer[3]=digits[int_digit%10];
		}
		else if (temp_f < 100.0) {
			int_digit=(temp_f*10.0);
			display_buffer[0]=digits[int_digit/100];
			int_digit-=(int_digit/100)*100;
			display_buffer[1]=digits[int_digit/10]|0x80; // set dp
			display_buffer[3]=digits[int_digit%10];
		}
		else {
			int_digit=(temp_f);
			display_buffer[0]=digits[int_digit/100];
			int_digit-=(int_digit/100)*100;
			display_buffer[1]=digits[int_digit/10];
			display_buffer[3]=digits[int_digit%10];
		}
		/* should we handle the 999+ degree case? */

		display_buffer[4]=0x63;			// degrees

		update_display(i2c_fd,HT16K33_ADDRESS1,display_buffer);

		sleep(5);
	}

	return 0;

}

