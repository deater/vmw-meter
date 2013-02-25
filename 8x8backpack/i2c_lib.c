/* Makes a spinning pattern on the display */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include <linux/i2c-dev.h>

#include "i2c_lib.h"

#define HT16K33_REGISTER_DISPLAY_SETUP		0x80
#define HT16K33_REGISTER_SYSTEM_SETUP		0x20
#define HT16K33_REGISTER_DIMMING		0xE0

/* Blink rate */
#define HT16K33_BLINKRATE_OFF			0x00
#define HT16K33_BLINKRATE_2HZ			0x01
#define HT16K33_BLINKRATE_1HZ			0x02
#define HT16K33_BLINKRATE_HALFHZ		0x03

#define HT16K33_ADDRESS				0x70

//static unsigned char subaddress=0;
//static int brightness=5;

static int display_fd=-1;


void shutdown_display(void) {
   if (display_fd>=0) {
      close(display_fd);
   }
}



void reset_display(unsigned short *display_state) {
  int i;

  for(i=0;i<DISPLAY_LINES;i++) {
     display_state[i]=0;
  }
}

void update_display(unsigned char *display_state) {

   unsigned char buffer[17];

   int i;

   /* only update if there's been a change */
//     if ( (existing_state[i*2]!=state[i*2]) ||
//        (existing_state[(i*2)+1]!=state[(i*2)+1])) {

//        existing_state[i*2]=state[i*2];
//        existing_state[(i*2)+1]=state[(i*2)+1];

      /* ugh bug in backpack?  bit 0 is actually LED 1, bit 128 LED 0 */
      /* Verify that somehow the python code is outputting like this */

      /* write out to hardware */
   buffer[0]=0x00;
   for(i=0;i<DISPLAY_LINES;i++) {
      buffer[(i*2)+1]=(display_state[i]>>1)|((display_state[i]&1)<<7);
      buffer[(i*2)+2]=0x00;
   }
   if ( (write(display_fd, buffer, 17)) !=17) {
      fprintf(stderr,"Erorr writing display!\n");
      exit(1);
   }




//    # Turn blink off
//    self.setBlinkRate(self.__HT16K33_BLINKRATE_OFF)

//    # Set maximum brightness
//    self.setBrightness(15)

//    # Clear the screen
//    self.clear()

}


/* should make the device settable */
int init_display(void) {

   unsigned char buffer[17];

   display_fd = open("/dev/i2c-1", O_RDWR);
   if (display_fd < 0) {
      fprintf(stderr,"Error opening i2c dev file /dev/i2c-1\n");
      return -1;
   }

   if (ioctl(display_fd, I2C_SLAVE, HT16K33_ADDRESS) < 0) {
      fprintf(stderr,"Error setting i2c address %x\n",HT16K33_ADDRESS);
      return -1;
   }

   /* Turn the oscillator on */
   buffer[0]= HT16K33_REGISTER_SYSTEM_SETUP | 0x01;
   if ( (write(display_fd, buffer, 1)) !=1) {
      fprintf(stderr,"Error starting display!\n");
      return -1;
   }

   /* Turn Display On, No Blink */
   buffer[0]= HT16K33_REGISTER_DISPLAY_SETUP | HT16K33_BLINKRATE_OFF | 0x1;
   if ( (write(display_fd, buffer, 1)) !=1) {
      fprintf(stderr,"Error starting display!\n");
      return -1;
   }

   return 0;
}
