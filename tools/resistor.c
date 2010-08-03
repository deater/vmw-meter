/* I used this program to brute force the 5/8 voltage divider    */
/* needed by the i2c chip as my hand-attempts never worked right */


#include <stdio.h>
#include <math.h>

#define NUMBER 48

double values[NUMBER]={
  100.0,110.0,120.0,130.0,150.0,160.0,180.0,
   200.0,220.0,240.0,270.0,
   300.0,330.0,360.0,390.0,
   430.0,470.0,
   510.0,560.0,
   620.0,680.0,
   750.0,
   820.0,
   910.0,
   1000.0,1100.0,1200.0,1300.0,1500.0,1600.0,1800.0,
   2000.0,2200.0,2400.0,2700.0,
   3000.0,3300.0,3600.0,
   4300.0,4700.0,
   5100.0,5600.0,
   6200.0,6800.0,
   7500.0,
   8200.0,
   9100.0,
   10000.0,
};

int main(int argc, char **argv) {
 
   int i,j;
   
   double divide;
   
   for(i=0;i<NUMBER;i++) {
      for(j=0;j<NUMBER;j++) {
	 divide=values[i]/(values[i]+values[j]);
	 
	 if (fabs(divide-(5.0/8.0))<0.01) {
	 
         printf("%lf = %lf (%lf %lf)\n",(5.0/8.0),
		divide,values[i],values[j]);  
	 }
      }
   }
   
   return 0;
}
