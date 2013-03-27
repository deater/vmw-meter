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

   int big_hack[8][8];

   int i,x,y,newi;

   /* only update if there's been a change */
//     if ( (existing_state[i*2]!=state[i*2]) ||
//        (existing_state[(i*2)+1]!=state[(i*2)+1])) {

//        existing_state[i*2]=state[i*2];
//        existing_state[(i*2)+1]=state[(i*2)+1];

      /* ugh bug in backpack?  bit 0 is actually LED 1, bit 128 LED 0 */
      /* Verify that somehow the python code is outputting like this */
      /* Fix bits mirrored error */

      for(y=0;y<8;y++) {
	 for(x=0;x<8;x++) {
            big_hack[x][y]=((display_state[y]>>(x))&1);
         }
      }

      /* write out to hardware */
   buffer[0]=0x00;
   for(i=0;i<DISPLAY_LINES;i++) {
      /* Fix off by one error */


      /* reconstruct */

	/* no rotate */
#if 0
      buffer[(i*2)+1]=0;
      buffer[(i*2)+1]|=big_hack[6][i]<<0;
      buffer[(i*2)+1]|=big_hack[5][i]<<1;
      buffer[(i*2)+1]|=big_hack[4][i]<<2;
      buffer[(i*2)+1]|=big_hack[3][i]<<3;
      buffer[(i*2)+1]|=big_hack[2][i]<<4;
      buffer[(i*2)+1]|=big_hack[1][i]<<5;
      buffer[(i*2)+1]|=big_hack[0][i]<<6;
      buffer[(i*2)+1]|=big_hack[7][i]<<7;
#endif
	/* rotate 270 degrees clockwise */
#if 0
      newi=i;
      if (newi==0) newi=7;
      else newi-=1;
      buffer[(newi*2)+1]=0;
      buffer[(newi*2)+1]|=big_hack[i][7]<<7;
      buffer[(newi*2)+1]|=big_hack[i][6]<<6;
      buffer[(newi*2)+1]|=big_hack[i][5]<<5;
      buffer[(newi*2)+1]|=big_hack[i][4]<<4;
      buffer[(newi*2)+1]|=big_hack[i][3]<<3;
      buffer[(newi*2)+1]|=big_hack[i][2]<<2;
      buffer[(newi*2)+1]|=big_hack[i][1]<<1;
      buffer[(newi*2)+1]|=big_hack[i][0]<<0;
#endif
      /* rotate 90 degrees clockwise */
      newi=i;
//      if (newi==0) newi=7;
//      else newi-=1;
      buffer[(newi*2)+1]=0;
      buffer[(newi*2)+1]|=big_hack[7-i][0]<<6;
      buffer[(newi*2)+1]|=big_hack[7-i][1]<<5;
      buffer[(newi*2)+1]|=big_hack[7-i][2]<<4;
      buffer[(newi*2)+1]|=big_hack[7-i][3]<<3;
      buffer[(newi*2)+1]|=big_hack[7-i][4]<<2;
      buffer[(newi*2)+1]|=big_hack[7-i][5]<<1;
      buffer[(newi*2)+1]|=big_hack[7-i][6]<<0;
      buffer[(newi*2)+1]|=big_hack[7-i][7]<<7;


      buffer[(newi*2)+2]=0x00;
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


/* Set brightness from 0 - 15 */
int set_brightness(int value) {

   unsigned char buffer[17];

   if ((value<0) || (value>15)) {
      fprintf(stderr,"Brightness value of %d out of range (0-15)\n",value);
      return -1;
   }

   /* Set Brightness */
   buffer[0]= HT16K33_REGISTER_DIMMING | value;
   if ( (write(display_fd, buffer, 1)) !=1) {
      fprintf(stderr,"Error setting brightness!\n");
      return -1;
   }

   return 0;
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

   set_brightness(5);

   return 0;
}

