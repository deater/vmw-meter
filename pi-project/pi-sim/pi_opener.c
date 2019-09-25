
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "pi-sim.h"
#include "pcx_load.h"
#include "print_string.h"

extern unsigned char _binary_pi_boot_pcx_start[];
extern unsigned char _binary_pi_boot_pcx_end[];

int main(int argc, char **argv) {

	unsigned char ch;
	unsigned char *pi_logo=_binary_pi_boot_pcx_start;
	int filesize=_binary_pi_boot_pcx_end-_binary_pi_boot_pcx_start;

	pisim_init();

	printf("Image is %d bytes\n",filesize);
	vmwPCXLoadPalette(pi_logo, filesize-769);
	vmwLoadPCX(pi_logo,100,100);

	print_string("Testing 123!",10,10,0xff);

	while(1) {

		ch=pisim_input();
		if (ch=='q') exit(0);
		if (ch==27) exit(0);
		if (ch==' ') {
			while(pisim_input()!=' ') usleep(10000);
		}
		pisim_update();
		usleep(10000);
	}

	return 0;
}
