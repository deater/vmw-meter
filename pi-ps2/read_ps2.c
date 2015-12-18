/* Reads data from a PS/2 keyboard on a Raspberry Pi	*/
/* connect GPIO23 (data) and GPIO24 (clk)		*/

/* By Vince Weaver <vincent.weaver _at_ maine.edu>	*/

#include <stdio.h>

#include <fcntl.h>	/* open() */
#include <unistd.h>	/* close() */
#include <string.h>
#include <errno.h>

	/* Exports a GPIO */
int export_gpio(int which) {

	int fd;
	char buffer[BUFSIZ];

	fd=open("/sys/class/gpio/export",O_WRONLY);
	if (fd<0) {
		printf("Error opening %s\n",strerror(errno));
	}
	else {
		sprintf(buffer,"%d",which);
		write(fd,buffer,strlen(buffer));
		close(fd);
	}

	return 0;
}

int set_input(int which) {

	int fd;
	char filename[BUFSIZ];

	sprintf(filename,"/sys/class/gpio/gpio%d/direction",which);

	/* Set direction to input */
	fd=open(filename,O_WRONLY);
	if (fd<0) {
		printf("Error direction %s\n",strerror(errno));
	}
	else {
		write(fd,"in",2);
		close(fd);
	}

	return 0;
}

int open_gpio(int which) {

	int fd;
	char filename[BUFSIZ];

	sprintf(filename,"/sys/class/gpio/gpio%d/value",which);

        /* Read switch */
        fd=open(filename,O_RDONLY);
        if (fd<0) {
		printf("Error reading %s\n",strerror(errno));
	}
	return fd;

}

int main(int argc, char **argv) {

	int clk_fd,data_fd;
	char value;


	export_gpio(23);
	export_gpio(24);

	set_input(23);
	set_input(24);

	clk_fd=open_gpio(23);
	data_fd=open_gpio(24);

	while(1) {

		read(clk_fd,&value,1);
		lseek ( clk_fd ,0 , SEEK_SET );

		if (value=='1') continue;

		read(data_fd,&value,1);
		lseek ( data_fd ,0 , SEEK_SET );
		printf("%c\n",value);

		while(1) {
			read(clk_fd,&value,1);
			lseek ( clk_fd ,0 , SEEK_SET );
			if (value=='1') break;
		}

	}


	return 0;

}
