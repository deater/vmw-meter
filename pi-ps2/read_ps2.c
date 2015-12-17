/* Reads data from a PS/2 keyboard on a Raspberry Pi	*/
/* connect GPIO23 (data) and GPIO24 (clk)		*/

/* By Vince Weaver <vincent.weaver _at_ maine.edu>	*/

#include <stdio.h>

#include <fcntl.h>	/* open() */
#include <unistd.h>	/* close() */
#include <string.h>
#include <errno.h>

int main(int argc, char **argv) {

	int fd;
	int old_state=0;
	int state,new_state;
	char value;
	int state_history;

	/* Export the GPIO */
	fd=open("/sys/class/gpio/export",O_WRONLY);
	if (fd<0) {
		printf("Error opening %s\n",strerror(errno));
	}
	else {
		write(fd,"23",2);
		close(fd);
	}

	/* Set direction to input */
	fd=open("/sys/class/gpio/gpio23/direction",O_WRONLY);
	if (fd<0) {
		printf("Error direction %s\n",strerror(errno));
	}
	else {
		write(fd,"in",2);
		close(fd);
	}

        /* Read switch */
        fd=open("/sys/class/gpio/gpio23/value",O_RDONLY);
        if (fd<0) {
		printf("Error reading %s\n",strerror(errno));
	}

	state_history=0;

	while(1) {

		read(fd,&value,1);
		lseek ( fd ,0 , SEEK_SET );

		state=value-'0';

		state_history<<=1;
		state_history|=state;

		if ((state_history&0xf)==0) {
			new_state=0;

			if (new_state!=old_state) {
				printf("Switch up!\n");
			}
			old_state=new_state;
		}


		if ( (state_history&0xf)==0xf) {
			new_state=1;

			if (new_state!=old_state) {
				printf("Switch down!\n");
			}
			old_state=new_state;
		}

		usleep(10000);
	}


	return 0;

}
