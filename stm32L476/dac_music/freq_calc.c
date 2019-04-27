#include <stdio.h>
#include <stdint.h>

int main(int argc, char **argv) {

	uint32_t freq,step;

	freq=44300000/440;
	step=360*1000000/freq;

	printf("step=%d\n",step);

	return 0;
}
