/* Makes a spinning pattern on the display */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include <linux/i2c-dev.h>

#define DISPLAY_LINES 8

#define HT16K33_REGISTER_DISPLAY_SETUP		0x80
#define HT16K33_REGISTER_SYSTEM_SETUP		0x20
#define HT16K33_REGISTER_DIMMING		0xE0

/* Blink rate */
#define HT16K33_BLINKRATE_OFF			0x00
#define HT16K33_BLINKRATE_2HZ			0x01
#define HT16K33_BLINKRATE_1HZ			0x02
#define HT16K33_BLINKRATE_HALFHZ		0x03

#define HT16K33_ADDRESS				0x70

static unsigned char subaddress=0;
static int brightness=5;

static int display_fd=-1;


static void shutdown_display(void) {
   if (display_fd>=0) {


      close(display_fd);
   }
}



static void reset_display(unsigned short *display_state) {
  int i;

  for(i=0;i<DISPLAY_LINES;i++) {
     display_state[i]=0;
  }
}

static int current_address=-1;

void write_i2c(char *buffer, int length) {

  int i=0;

  int address;

  address=buffer[0]/2; 

  i++;
  if (i>length) return;

  if (address!=current_address) {

     if (ioctl(display_fd, I2C_SLAVE, address) < 0) {
        fprintf(stderr,"error setting i2c address\n");
        exit(1);
     }
     current_address=address;
  }

  if ( (write(display_fd, buffer+1, length-1)) !=length-1) {
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

void update_display(unsigned short *display_state) {


   int i;

     /* only update if there's been a change */
//     if ( (existing_state[i*2]!=state[i*2]) ||
  //        (existing_state[(i*2)+1]!=state[(i*2)+1])) {

//        existing_state[i*2]=state[i*2];
//        existing_state[(i*2)+1]=state[(i*2)+1];

	unsigned char buffer[16];

        /* write out to hardware */
   buffer[0]=HT16K33_ADDRESS;
   buffer[1]=0x00;
   for(i=0;i<DISPLAY_LINES;i++) {
      buffer[(i*2)+2]=(display_state[i])&0xff;
      buffer[(i*2)+3]=(display_state[i]>>8)&0xff;
   }
   write_i2c(buffer,8);
}


/* should make the device settable */
static void init_display(void) {

   unsigned char buffer[17];
   unsigned char display_buffer[8];
   int i,x,y;

   display_fd = open("/dev/i2c-1", O_RDWR);
   if (display_fd < 0) {
      fprintf(stderr,"Error opening i2c dev file /dev/i2c-1\n");
      exit(1);
   }

   if (ioctl(display_fd, I2C_SLAVE, HT16K33_ADDRESS) < 0) {
      fprintf(stderr,"Error setting i2c address %x\n",HT16K33_ADDRESS);
      exit(1);
   }

   /* Turn the oscillator on */
   buffer[0]= HT16K33_REGISTER_SYSTEM_SETUP | 0x01;
   if ( (write(display_fd, buffer, 1)) !=1) {
      fprintf(stderr,"Error starting display!\n");
      exit(1);
   }

   /* Turn Display On, No Blink */
   buffer[0]= HT16K33_REGISTER_DISPLAY_SETUP | HT16K33_BLINKRATE_OFF | 0x1;
   if ( (write(display_fd, buffer, 1)) !=1) {
      fprintf(stderr,"Error starting display!\n");
      exit(1);
   }

   /* Fill Demo */
   while(1) {
      for(i=0;i<DISPLAY_LINES;i++) display_buffer[i]=0;
      for(y=0;y<DISPLAY_LINES;y++) {
         for(x=0;x<DISPLAY_LINES;x++) {

/* ugh bug in backpack?  bit 0 is actually LED 1, bit 128 LED 0 */
/* Verify that somehow the python code is outputting like this */
usleep(50000);
   /* write out to hardware */
   buffer[0]=0x00;
   for(i=0;i<DISPLAY_LINES;i++) {
      buffer[(i*2)+1]=(display_buffer[i]>>1)|((display_buffer[i]&1)<<7);
      buffer[(i*2)+2]=0x00;
   }
   if ( (write(display_fd, buffer, 17)) !=17) {
      fprintf(stderr,"Erorr writing display!\n");
      exit(1);
   }

            display_buffer[y]<<=1;
            display_buffer[y]+=1;

}

}
 usleep(500000);
}


//    # Turn blink off
//    self.setBlinkRate(self.__HT16K33_BLINKRATE_OFF)

//    # Set maximum brightness
//    self.setBrightness(15)

//    # Clear the screen
//    self.clear()


}


int main(int argc, char **argv) {

  int i,spin=0,dot1=0,dot2=19,blob=0;
  unsigned short display_state[DISPLAY_LINES];

  init_display();
#if 0
  reset_display(display_state);
//  display_config_sane();

  display_state[0]=0xaaaa;
  display_state[1]=0x5555;

  update_display(display_state);
  //usleep(100000);
#endif
  return 0;
}

