#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "lpd8806.h"
#include "colors.h"
#include "effects.h"


static int help(char *executable) {

	printf("%s effect options\n",executable);
	printf("\tbargraph color1 color2\n");
	printf("\n");

	return 0;
}

#define EFFECT_RANDOM	0
#define EFFECT_BARGRAPH	1
#define EFFECT_RAINBOW	2

int main(int argc, char **argv) {

	int spi_fd;
	int effect=EFFECT_RANDOM;

	if (argc>1) {
		if (!strncmp(argv[1],"-h",2)) {
			help(argv[0]);
			return 0;
		}

		if (!strncmp(argv[1],"bargraph",8)) {
			effect=EFFECT_BARGRAPH;
		}

		if (!strncmp(argv[1],"rainbow",7)) {
			effect=EFFECT_RAINBOW;
		}

	}

	spi_fd=lpd8806_init();
	if (spi_fd<0) {
		exit(-1);
	}

	switch(effect) {
		case EFFECT_BARGRAPH:
			bargraph(spi_fd,argc>2?argv[2]:NULL,
					argc>3?argv[3]:NULL);
			break;
		case EFFECT_RAINBOW:
			rainbow(spi_fd);
			break;
	}

	lpd8806_close(spi_fd);

	return 0;
}
