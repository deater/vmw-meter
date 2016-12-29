#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "lpd8806.h"
#include "colors.h"
#include "effects.h"


static int help(char *executable) {

	printf("%s effect options\n",executable);
	printf("\tbargraph color1 color2 -- alternate two colors\n");
	printf("\tdisable -- set display to black\n");
	printf("\tfish num -- moving fish-like dots\n");
	printf("\tpulsar color -- supernova\n");
	printf("\trainbow -- rainbow pattern \n");
	printf("\tscanner color -- colored bar that bounces back and forth\n");
	printf("\t\tscanner_blinky -- blinky scanner\n");
	printf("\t\tscanner_dual color1 color2 -- two color scanner\n");
	printf("\t\tscanner_random -- scanner with random color changes\n");
	printf("\tstars speed brightness -- star pattern\n");
	printf("\ttwo_color_scroll color1 color2 direction\n");
	printf("\t\tred_green -- two_color_scroll with red/green default\n");
	printf("\t\tblue_yellow -- two_color_scroll with blue/yellow default\n");
	printf("\n");

	return 0;
}

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

		if (!strncmp(argv[1],"disable",7)) {
			effect=EFFECT_DISABLE;
		}

		if (!strncmp(argv[1],"fish",4)) {
			effect=EFFECT_FISH;
		}

		if (!strncmp(argv[1],"pulsar",6)) {
			effect=EFFECT_PULSAR;
		}


		if (!strncmp(argv[1],"rainbow",7)) {
			effect=EFFECT_RAINBOW;
		}

		if (!strncmp(argv[1],"stars",5)) {
			effect=EFFECT_STARS;
		}

		if (!strncmp(argv[1],"red_green",9)) {
			effect=EFFECT_RED_GREEN;
		}

		if (!strncmp(argv[1],"blue_yellow",11)) {
			effect=EFFECT_BLUE_YELLOW;
		}

		if (!strncmp(argv[1],"two_color_scroll",15)) {
			effect=EFFECT_TWO_COLOR_SCROLL;
		}

		if (!strncmp(argv[1],"scanner_random",14)) {
			effect=EFFECT_SCANNER_RANDOM;
		}

		if (!strncmp(argv[1],"scanner",7)) {
			effect=EFFECT_SCANNER;
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

		case EFFECT_DISABLE:
			disable(spi_fd);
			break;

		case EFFECT_FISH:
			fish(spi_fd,argc>2?argv[2]:NULL);
			break;

		case EFFECT_PULSAR:
			pulsar(spi_fd,argc>2?argv[2]:NULL);
			break;

		case EFFECT_RAINBOW:
			rainbow(spi_fd);
			break;

		case EFFECT_STARS:
			stars(spi_fd,argc>2?argv[2]:NULL,
					argc>3?argv[3]:NULL);
			break;
		case EFFECT_TWO_COLOR_SCROLL:
			two_color_scroll(spi_fd,
					argc>2?argv[2]:NULL,
					argc>3?argv[3]:NULL,
					argc>4?argv[4]:NULL);
			break;

		case EFFECT_RED_GREEN:
			two_color_scroll(spi_fd,
					"red",
					"green",
					"1");
			break;
		case EFFECT_BLUE_YELLOW:
			two_color_scroll(spi_fd,
					"blue",
					"yellow",
					"0");
			break;

		case EFFECT_SCANNER:
			scanner(spi_fd,argc>2?argv[2]:NULL);
			break;

		case EFFECT_SCANNER_BLINKY:
			scanner_blinky(spi_fd);
			break;

		case EFFECT_SCANNER_DUAL:
			scanner_dual(spi_fd,
				argc>2?argv[2]:NULL,
				argc>3?argv[3]:NULL);
			break;

		case EFFECT_SCANNER_RANDOM:
			scanner_random(spi_fd);
			break;


	}

	lpd8806_close(spi_fd);

	return 0;
}
