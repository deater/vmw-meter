/* Reads data from a PS/2 keyboard on a Raspberry Pi	*/
/* connect GPIO23 (data) and GPIO24 (clk)		*/

/* By Vince Weaver <vincent.weaver _at_ maine.edu>	*/

#include <stdio.h>

#include <fcntl.h>	/* open() */
#include <unistd.h>	/* close() */
#include <string.h>
#include <stdlib.h>

#include <sys/mman.h>

#include <errno.h>

#define GPIO_LEN	0xb4
#define GPIO_BASE	0x20200000
#define GPFLEV0 	13

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

volatile unsigned int *regAddrMap = MAP_FAILED;


int read_gpio(int which) {
	int result=0;

        if ((*(regAddrMap + GPFLEV0) & (1 << which)) != 0)
                result = 1;

        return result;

}

int keymap[128]={
//	 0   1   2   3   4   5   6   7   8   9   a   b   c   d   e   f
	'0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0', // 00
	'0','0','0','0','0','q','0','0','0','0','z','s','a','w','0','0', // 10
	'0','c','x','d','e','0','0','0','0','0','v','f','t','r','0','0', // 20
	'0','n','b','h','g','y','0','0','0','0','m','j','u','0','0','0', // 30
	'0','0','k','i','o','0','0','0','0','0','0','l','0','p','0','0', // 40
	0,0,0,0,0,0,0,0, // 50
	0,0,0,0,0,0,0,0, // 58
	0,0,0,0,0,0,0,0, // 60
	0,0,0,0,0,0,0,0, // 68
	0,0,0,0,0,0,0,0, // 70
	0,0,0,0,0,0,0,0, // 78
};


int decode_keycode(int key) {
	if (key<0x80) return keymap[key];
	return 0;

}

int decode_key(int message) {

	int parity;
	int check=0;
	int keycode=0;
	int i;

	if (message&0x400) {
		printf("Invalid start bit %x!\n",message);
	}
	if (!(message&0x1)) {
		printf("Invalid stop bit!\n");
	}

	parity=!!(message&0x2);

	for(i=2;i<10;i++) {
		if (message&(1<<i)) {
			keycode|=1<<(9-i);
			check++;
		}
	}

	if (!((check+parity)&1)) {
		printf("Parity error check=%d parity=%d!\n",check,parity);
	}

//	printf("Message=%x Keycode=%x\n",message,keycode);

	printf("Message=%x Keycode=%x Decoded='%c'\n",message,keycode,decode_keycode(keycode));

	return keycode;


}

int main(int argc, char **argv) {

	volatile int value;

	int mem_fd;

	int clock_bits=0;
	int message=0;

	mem_fd = open("/dev/mem", O_RDWR|O_SYNC);
	if (mem_fd<0) {
		printf("Couldn't open /dev/mem %s\n",strerror(errno));
		exit(1);
	}

	/* mmap IO */
	regAddrMap = mmap(
		NULL,             //Any adddress in our space will do
		GPIO_LEN,       //Map length
		PROT_READ|PROT_WRITE|PROT_EXEC,// Enable reading & writting to mapped memory
		MAP_SHARED|MAP_LOCKED,       //Shared with other processes
		mem_fd,           //File to map
		GPIO_BASE         //Offset to base address
	);

	if (regAddrMap == MAP_FAILED) {
          	printf("mmap error %s",strerror(errno));
          	close(mem_fd);
          	exit (1);
	}

	close(mem_fd);

	export_gpio(23);
	export_gpio(24);

	set_input(23);
	set_input(24);

	while(1) {

		/* normally high */
		value=read_gpio(23);
		if (value) continue;

		clock_bits++;

		value=read_gpio(24);
		message<<=1;
		message|=value;


		if (clock_bits==11) {
			decode_key(message);
			clock_bits=0;
			message=0;
		}

		while(1) {
			value=read_gpio(23);
			if (value) break;
		}



	}


	return 0;

}
