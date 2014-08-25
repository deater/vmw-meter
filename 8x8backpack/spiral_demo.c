/* Makes a spinning pattern on the display */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "i2c_lib.h"
#include "graphics_lib.h"


struct coord_list {
  int x;
  int y;
} spiral_coords[]={
   {0,7},
   {1,7},
   {2,7},
   {3,7},
   {4,7},
   {5,7},
   {6,7},
   {7,7},

   {7,6},
   {7,5},
   {7,4},
   {7,3},
   {7,2},
   {7,1},
   {7,0},
   {6,0},

   {5,0},
   {4,0},
   {3,0},
   {2,0},
   {1,0},
   {0,0},
   {0,1},
   {0,2},

   {0,3},
   {0,4},
   {0,5},
   {0,6},
   {1,6},
   {2,6},
   {3,6},
   {4,6},

   {5,6},
   {6,6},
   {6,5},
   {6,4},
   {6,3},
   {6,2},
   {6,1},
   {5,1},

   {4,1},
   {3,1},
   {2,1},
   {1,1},
   {1,2},
   {1,3},
   {1,4},
   {1,5},

   {2,5},
   {3,5},
   {4,5},
   {5,5},
   {5,4},
   {5,3},
   {5,2},
   {4,2},

   {3,2},
   {2,2},
   {2,3},
   {2,4},
   {3,4},
   {4,4},
   {4,3},
   {3,3},

   {-1,-1},
};

/* should make the device settable */
int spiral_demo(int i2c_fd) {

	int i;
	unsigned char display_buffer[DISPLAY_LINES];

	while(1) {
		for(i=0;i<DISPLAY_LINES;i++) display_buffer[i]=0;

		i=0;
		while(spiral_coords[i].x!=-1) {

			usleep(50000);

			plotxy(display_buffer,spiral_coords[i].x,
					spiral_coords[i].y);
			update_8x8_display_rotated(i2c_fd,HT16K33_ADDRESS0,
				display_buffer,0,BROKEN,GREEN_PLANE);
			i++;
		}
		usleep(500000);
	}

	return 0;
}


int main(int argc, char **argv) {

	int result;
	int i2c_fd;

	i2c_fd=init_i2c("/dev/i2c-1");
	if (i2c_fd < 0) {
		fprintf(stderr,"Error opening device!\n");
		return -1;
	}

	/* Init display */
	if (init_display(i2c_fd,HT16K33_ADDRESS0,10)) {
		fprintf(stderr,"Error opening display\n");
		return -1;
	}

	result=spiral_demo(i2c_fd);

	return result;
}

