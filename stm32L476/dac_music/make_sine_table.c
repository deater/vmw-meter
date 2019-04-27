#include <stdio.h>
#include <math.h>

int main(int argc, char **argv) {

	double sf;
	signed int sine_table[91];
	int i;

	for(i=0;i<=90;i++) {

		sf=sin(M_PI * (i/180.0));
		sine_table[i]=(1+sf)*2048;
		if (sine_table[i] == 0x1000) {
			sine_table[i]=0xfff;
		}
	}

	printf("int sine_lookup[91]={");
	for(i=0;i<=90;i++) {
		if (i%10==0) printf("\n\t/* %02d */ ",i);
		printf("0x%03x,",sine_table[i]);
	}
	printf("\n};\n");

	return 0;
}
