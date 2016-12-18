/* Makes a timer */

/* Assumes 4x7 LED display on i2c */
/* Assumes push buttons on GPIO17 and GPIO27 */
/*   (pulled to ground on one side, pulled up to 3.3V on other) */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <time.h>

#include "i2c_lib.h"

static unsigned short digits[16] = {
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

struct button_state {
	int fd;
	int gpio;
	int history;
	int current_state;
	int old_state;
	int pressed;
};

static int init_button(int which, struct button_state *state) {

	int fd;
	char string[BUFSIZ];

	state->history=0;
	state->current_state=0;
	state->old_state=0;
	state->gpio=which;
	state->pressed=0;

	sprintf(string,"%d",which);

	/* Export the GPIO */
	fd=open("/sys/class/gpio/export",O_WRONLY);
	if (fd<0) {
		printf("Error exporting %d, %s\n",which,strerror(errno));
		return -1;
	}
	else {
		write(fd,string,2);
		close(fd);
	}

	/* Set direction to input */
	sprintf(string,	"/sys/class/gpio/gpio%d/direction",which);

	fd=open(string,O_WRONLY);
	if (fd<0) {
		printf("Error direction %d, %s\n",which,strerror(errno));
		return -1;
	}
	else {
		write(fd,"in",2);
		close(fd);
	}

        /* Open Value */
        sprintf(string,"/sys/class/gpio/gpio%d/value",which);
        fd=open(string,O_RDONLY);
        if (fd<0) {
		printf("Error opening %d, %s\n",which,strerror(errno));
		return -1;
	}

	state->fd=fd;

	return 0;
}

static int read_button(struct button_state *state) {

	int new_state;
	char value;

	read(state->fd,&value,1);
	lseek ( state->fd ,0 , SEEK_SET );

	state->current_state=value-'0';

//	printf("GPIO: %d state=%d\n",state->gpio,state->current_state);

	state->history<<=1;
	state->history|=state->current_state;

	if ((state->history&0x7)==0) {

		new_state=0;

		if (new_state!=state->old_state) {
//			printf("Switch up!\n");
			state->pressed=1;
		}
		state->old_state=new_state;
	}


	if ( (state->history&0x7)==0x7) {
		new_state=1;

		if (new_state!=state->old_state) {
//			printf("Switch down!\n");
			state->pressed=0;
		}
		state->old_state=new_state;
	}

	return 0;

}



int main(int argc, char **argv) {

	unsigned short display_buffer[8];
	int i;
	int blink=0,running;
	int i2c_fd;
	struct button_state buttons[2];

	time_t start_time,current_time,elapsed;
	int minutes,seconds,old_seconds=0;
	int total_time=0;

	/* Init i2c */
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

	/* Init buttons */
	if (init_button(17,&buttons[0])<0) {
		fprintf(stderr,"Error opening buttons\n");
		return -1;
	}

	if (init_button(27,&buttons[1])<0) {
		fprintf(stderr,"Error opening buttons\n");
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

	running=0;
	minutes=0;
	seconds=0;

	while(1) {

		/* clear display */
		for(i=0;i<8;i++) display_buffer[i]=0x00;

		if (running) {
			current_time=time(NULL);
			elapsed=total_time+(current_time-start_time);
			minutes=elapsed/60;
			seconds=elapsed%60;
		}

		/* minutes */
		display_buffer[0]=digits[(minutes/ 10)];
		display_buffer[1]=digits[(minutes% 10)];

		/* seconds */
		display_buffer[3]=digits[(seconds / 10)];
		display_buffer[4]=digits[(seconds % 10)];

		if (seconds!=old_seconds) blink=0;
		old_seconds=seconds;

		blink++;
		if (blink>5) {
			display_buffer[2]|=0x02;
		}
		if (blink>10) {
			blink=0;
		}

		update_display(i2c_fd,HT16K33_ADDRESS0,display_buffer);

		read_button(&buttons[0]);
		read_button(&buttons[1]);

		/* start */
		if (buttons[0].pressed) {
			if (running==0) {
				printf("Starting Timer!\n");
				start_time=time(NULL);
				running=1;
			}
			else {
				printf("Stopping Timer!\n");
				total_time=elapsed;
				running=0;
			}
			buttons[0].pressed=0;


		}

		/* reset */
		if (buttons[1].pressed) {
			printf("Resetting timer!\n");
			running=0;
			minutes=0;
			seconds=0;
			total_time=0;
			buttons[1].pressed=0;

		}

		usleep(100000);
	}

	return 0;

}


