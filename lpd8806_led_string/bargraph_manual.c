#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <math.h>

#include "effects.h"
#include "lpd8806.h"
#include "colors.h"


int bargraph_manual(int spi_fd, char *color1, char *color2) {

	int i;
	unsigned char data[128];

	int bar=0;
	unsigned char ch;

	int lr,lg,lb,rr,rg,rb;

	/* color left */
	if (color1) {
                get_color(color1,&lr,&lg,&lb);
        }
        else {
                lr=63;
                lg=0;
                lb=0;
        }


	/* color right */
	if (color2) {
                get_color(color2,&rr,&rg,&rb);
        }
        else {
                rr=0;
                rg=0;
                rb=0;
        }

	/* Clear to black */
	for(i=0;i<128;i++) data[i]=128;

	while(1) {
		scanf("%c",&ch);

		printf("Pressed %d\n",ch);

		if (ch=='i') {
			bar++;
			if (bar>32) bar=32;
		}
		if (ch=='m') {
			bar--;
			if (bar<0) bar=0;
		}

		for(i=0;i<32;i++) {
			if (i<bar) {
				data[(i*3)+0]=128+lg;
				data[(i*3)+1]=128+lr;
				data[(i*3)+2]=128+lb;
			} else {
				data[(i*3)+0]=128+rg;
				data[(i*3)+1]=128+rr;
				data[(i*3)+2]=128+rb;
			}
		}

		lpd8806_write(spi_fd,data);

	}

	lpd8806_close(spi_fd);

	return 0;
}
