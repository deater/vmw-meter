/* this tool is used to help design fonts for the 14-seg display */

#include <stdio.h>

#include "14seg_font.h"
#include "i2c_lib.h"

void print_char(short value) {

	if (value & SAA1064_SEGMENT_A) printf(" ___ \n");
	else printf("     \n");

	if (value & SAA1064_SEGMENT_F) printf("|");
	else printf(" ");
	if (value & SAA1064_SEGMENT_P) printf("\\");
	else printf(" ");
	if (value & SAA1064_SEGMENT_G) printf("|");
	else printf(" ");
	if (value & SAA1064_SEGMENT_H) printf("/");
	else printf(" ");
	if (value & SAA1064_SEGMENT_B) printf("|\n");
	else printf(" \n");

	if (value & SAA1064_SEGMENT_N) printf(" - ");
	else printf("   ");
	if (value & SAA1064_SEGMENT_J) printf("- \n");
	else printf("  \n");

	if (value & SAA1064_SEGMENT_E) printf("|");
	else printf(" ");
	if (value & SAA1064_SEGMENT_M) printf("/");
	else printf(" ");
	if (value & SAA1064_SEGMENT_L) printf("|");
	else printf(" ");
	if (value & SAA1064_SEGMENT_K) printf("\\");
	else printf(" ");
	if (value & SAA1064_SEGMENT_C) printf("|\n");
	else printf(" \n");

	if (value & SAA1064_SEGMENT_D) printf(" --- \n");
	else printf("     \n");

  /*
 ___
|\|/|
 - -
|/|\|
 ---

  */
	return;
}

int main(int argc, char **argv) {

	int i;

	for(i=0;i<256;i++) {
		printf("%x ",i);
		if ((i>31 ) && (i< 127)) printf("'%c'",i);
		printf("\n");
		print_char(ascii_lookup[i]);
	}
	return 0;
}
