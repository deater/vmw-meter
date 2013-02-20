/* code to drive the LED display */

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include <linux/i2c-dev.h>

#include "meter.h"

#define METER_DIGITS 8
#define CHIPS 4

static unsigned short meter_state[METER_DIGITS];
static unsigned char config_state[CHIPS];
static unsigned char subaddress=0;
static int led_current[CHIPS];

static int meter_fd=-1;

// rmmod lp, ppdev
// insmod parport-pc
// sudo /sbin/modprobe i2c-parport type=3
// /sys/class/i2c-adapter
// /sbin/modprobe  i2c_dev    ( or device files won't show)


/* should make the device settable */
void init_meter() {
   
   meter_fd = open("/dev/i2c-1", O_RDWR);
   if (meter_fd < 0) {
      fprintf(stderr,"Error opening i2c dev file /dev/i2c-1\n");
      exit(1);
   }
   
}

void shutdown_meter() {
   if (meter_fd>=0) {	
      close(meter_fd);
   }  
}



void reset_display() {
  int i;

  for(i=0;i<CHIPS;i++) {
     config_state[i]=0;
     led_current[i]=0;
  }
  subaddress=0;
  for(i=0;i<METER_DIGITS;i++) {
    meter_state[i]=0;
  }

}

static int current_address=-1;

void emulate_i2c(char *buffer, int length) {

  int i=0;

  int address;
 
  address=buffer[0]/2; /* linux i2c expects addres / 2 */

//  if (address>0x39) return;
   
  i++;
  if (i>length) return;
   
  if (address!=current_address) {
	
     if (ioctl(meter_fd, I2C_SLAVE, address) < 0) {
        fprintf(stderr,"error setting i2c address\n");
        exit(1);
     }
     current_address=address;  
  }


// char *buf=buffer+2;
//  printf("Writing S %x A %x ",address,buffer[1]);
//  for(i=0;i<length-2;i++) {
//     printf("%x ",buf[i]);
//  }
//  printf("\n");
  
  if ( (write(meter_fd, buffer+1, length-1)) !=length-1) {
     fprintf(stderr,"Warning, cannot write %x (%x)\n", address,address*2);
//     exit(1);
  }

#if 0    
  i2c_smbus_write_block_data(meter_fd,
			     buffer[1],
			     length-2,
			     buffer+2);
   #endif
}

