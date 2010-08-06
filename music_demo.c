#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

#include "meter.h"
#include "meter_tools.h"

/* based on the vu code by Toby Shepard */
/*   http://tobiah.org/pub/vu/          */
   
#define SAMPLE_RATE 44100
#define SAMPLE_WIDTH 16
#define NUM_CHANNELS  2

#define DISPLAY_RATE 10

unsigned short bargraph_lut[11]=
{0x0000,
         0x8000,
         0xc000,
         0xe000,
         0xf000,
         0xf800,
         0xfc00,
         0xfe00,
         0xff00,
         0xff80,
         0xffc0
};


int main(int argc, char **argv) {

   double maximum;
   int best[NUM_CHANNELS], counter, flag = 1, channel;
   short holder[2], temp_holder;
   char name[7];
   double crude;
   long time = 0;

   unsigned short display_state[8];
   int i;
   
      /* Init Display */
   init_meter();
   reset_display();
   display_config_sane();
   for(i=0;i<8;i++) { display_state[i]=0; }
   update_display(display_state);   

   strncpy(name,argv[1],6);
   
   /***** REALLY THE MAX ALLOWABLE AMPLITUDE *****/
   //divisor = pow(2, (float)SAMPLE_WIDTH ) / 2.0 - 1.0;
   maximum = ((1<<SAMPLE_WIDTH ) / 2) - 1;

   /***** READ TO END OF INPUT *****/
   while(flag) {

      /***** DO A VISUAL BLOCK *****/
      for(counter = 0; counter <= (SAMPLE_RATE / DISPLAY_RATE); counter++) {

	 /***** DO A SAMPLE FRAME *****/
	 for(channel = 0; channel < NUM_CHANNELS; channel++){
	    best[channel] = 0;
	    holder[channel] = 0;
	    /***** READ A SAMPLE *****/
	    if (fread(&holder[channel] , 
		      (SAMPLE_WIDTH / 8), 1, stdin) != 1) {
	       flag = 0;
	       break;
	    }
	    time++; 

	    /***** CORRECT SAMPLE AND TEST FOR GREATEST VALUE SO FAR *****/
	    temp_holder = (SAMPLE_WIDTH == 8 ? holder[channel] - 128 : 
			                             holder[channel]);
	    if (temp_holder < -maximum){
	       temp_holder = -maximum;
	    }
				
	    if ( abs(temp_holder) > best[channel]) {
	       best[channel] = abs(temp_holder);
	    }

	 }

	 
	 for(channel = 0; channel < NUM_CHANNELS; channel++){
	    /***** WRITE THE SAMPLE BACK OUT *****/
	    if (fwrite(&holder[channel] , SAMPLE_WIDTH/8, 1 , stderr) != 1){
	       flag = 0;
	       break;
	    }
	 }
	 
	 
      }

      for(channel=0;channel<NUM_CHANNELS;channel++) {
         /***** FIND FRACTION OF MAX AMPLITUDE *****/
         crude = sqrt( (double)(best[channel] ) / maximum) ;
         if (crude > 1){
	    crude = 1;
         }
	 
	 
#if 0
         /***** CREATE DISPLAY BAR INFORMATION *****/
         strcpy(canvas, scale);
         memset(canvas, 'O', (int)(crude * 50.0));
         sprintf(message2, "%2d%%", (int)(crude * (100.0) ) );
         sprintf(message3, "%d", best[channel]); 

         /***** OUTPUT DISPLAY BAR *****/
         fprintf(stderr, "%s ", canvas);

         fprintf(stderr, "T:%-7.4g ", ((double)time / (double)SAMPLE_RATE)
	      / NUM_CHANNELS);

         fprintf(stderr, "\n");
#endif	 
	 
	 display_state[6+channel]=bargraph_lut[(int)(crude*10.0)];         
      }
      update_display(display_state);
      for(i=0;i<6;i++) {
	 display_state[i]=ascii_lookup[(unsigned char)name[i]];
      }
   }

   /* clear display */
   for(i=0;i<8;i++) { display_state[i]=0; }
   update_display(display_state);   
   
   return 0;
}
