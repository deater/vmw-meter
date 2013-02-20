/* Code to emulate the led meter using ANSI color ASCII art */

#include <stdio.h>
#include <string.h>

#include "meter.h"

#define METER_DIGITS 8
#define CHIPS 4

static unsigned short global_meter_state[METER_DIGITS];
static unsigned char config_state[CHIPS];
static unsigned char subaddress=0;
static int led_current[CHIPS];

static void print_meter(void);

/* nothing needed */
void init_meter() {
 
}

/* set all segments to zero */
void reset_display() {
  int i;

  for(i=0;i<CHIPS;i++) {
     config_state[i]=0;
     led_current[i]=0;
  }
  subaddress=0;
  for(i=0;i<METER_DIGITS;i++) {
    global_meter_state[i]=0;
  }
  print_meter();
}

void emulate_i2c(char *buffer, int length) {

  int i=0;

  //for(i=0;i<length;i++) {
  //  printf("%d: %x\n",i,buffer[i]);
  //}

  int chip,address,digit;

  i++;
  if (i>length) return;
 
  chip=(buffer[0]-0x70)>>1;
  //    printf("Chip %d\n",chip);

  i++;
  if (i>length) return;

  address=buffer[1]&0x7;
  //printf("Address %d\n",address);

  while(i<length) {
    //printf("Addr %d = %x\n",address,buffer[i]);
       
    digit=(chip*2)+(1-(address%2));
     
    switch(address) {
    case 0: 
            config_state[chip]=buffer[i];
            led_current[chip]=(!!(config_state[chip]&CONTROL_PLUS3MA))*3+
	      (!!(config_state[chip]&CONTROL_PLUS6MA))*6+
	      (!!(config_state[chip]&CONTROL_PLUS12MA))*12;
            //printf("LED Current for chip %d = %dmA\n",chip,led_current[chip]);
            break;
    case 1:
    case 2: 
       global_meter_state[digit]=(global_meter_state[digit]&0x00ff) | (buffer[i]<<8);
       break;
    case 3: 
    case 4: 
       global_meter_state[digit]=(global_meter_state[digit]&0xff00) | (buffer[i]&0xff);   
       break;
    default: break;  /* 5 - 7 are reserved */
    }

    i++;
    address++;
    if (address>7) address=0;
  }
  print_meter();
}

static void red_or_grey(int condition) {
  if (condition) {
    /* bright red */
    printf("\033[1;31m");
  }
  else {
    /* dark grey */
    printf("\033[1;30m");
  }
}

static void bargraph(int condition1, int condition2) {

  if (condition1 && condition2) {
    /* bright green  */
    printf("\033[1;32m");
     printf(";; ");
  }
  else if (condition1) {
    /* bright green  */
    printf("\033[1;32m");
     printf(",, ");
  }
  else if (condition2) {
    /* bright green  */
    printf("\033[1;32m");
     printf("'' ");
  }
  else {
    /* dark grey */
    printf("\033[1;30m");
    printf(";; ");
  }



}

static unsigned short reverse_bits16(unsigned short v) {
   
         unsigned int s = 16;
         unsigned int mask = ~0;
         while ((s >>= 1) > 0) {
	                   mask ^= (mask << s);
	                   v = ((v >> s) & mask) | ((v << s) & ~mask);
	 }
     
         return v;
}



static void print_meter() {

  int i;
   
  unsigned short meter_state[METER_DIGITS],temp;
   
  for(i=0;i<8;i++) {
     meter_state[i]=global_meter_state[i];
  }
   
  /* workaround bug in hardware where bargraph wired backwards */
   
  temp=(reverse_bits16(meter_state[6])<<6)|(meter_state[6]&0x3f);
  meter_state[6]=temp;
	
  temp=(reverse_bits16(meter_state[7])<<6)|(meter_state[7]&0x3f);
  meter_state[7]=temp;
   
   
  //  for(i=0;i<8;i++) printf("%i %x\n",i,meter_state[i]);

  /* clear screen */
  printf("\033[2J\n");

  /* move to upper left */
  printf("\033[1;1H");

  //                @ @ @ @ @ @
  //     @ @   @ @   @ @   @ @   @ @   @ @
  // ;;  ___   ___   ___   ___   ___   ___  ;;
  // ;; |\|/| |\|/| |\|/| |\|/| |\|/| |\|/| ;;
  // ;;  - -   - -   - -   - -   - -   - -  ;;
  // ;; |/|\| |/|\| |/|\| |/|\| |/|\| |/|\| ;;
  // ;;  ---   ---   ---   ---   ---   ---  ;;
  //

  /* first line */
  printf("                ");

  for(i=0;i<6;i++) {
     if (meter_state[i]&SEGMENT_EX) {
       switch(i) {
       case 0: printf("\033[0;31m"); break; /* bright red */
       case 1: printf("\033[1;31m"); break; /* bright orange */
       case 2: printf("\033[1;33m"); break; /* bright yellow */
       case 3: printf("\033[1;32m"); break; /* bright green */
       case 4: printf("\033[1;34m"); break; /* bright blue */
       case 5: printf("\033[1;35m"); break; /* bright purple */
       }
     }
     else {
        /* dark grey */
        printf("\033[1;30m");
     }
     printf("@ ");
  }
  printf("\n");

  /* second line */
  printf("     ");
  red_or_grey(meter_state[6]&SEGMENT_L);
  printf("@ ");
  red_or_grey(meter_state[6]&SEGMENT_M);
  printf("@   ");
  red_or_grey(meter_state[6]&SEGMENT_N);
  printf("@ ");
  red_or_grey(meter_state[6]&SEGMENT_P);
  printf("@   ");
  red_or_grey(meter_state[6]&SEGMENT_DP);
  printf("@ ");
  red_or_grey(meter_state[6]&SEGMENT_EX);
  printf("@   ");

  red_or_grey(meter_state[7]&SEGMENT_EX);
  printf("@ ");
  red_or_grey(meter_state[7]&SEGMENT_DP);
  printf("@   ");
  red_or_grey(meter_state[7]&SEGMENT_P);
  printf("@ ");
  red_or_grey(meter_state[7]&SEGMENT_N);
  printf("@   ");
  red_or_grey(meter_state[7]&SEGMENT_M);
  printf("@ ");
  red_or_grey(meter_state[7]&SEGMENT_L);
  printf("@");
  printf("\n");

  /* third line */
  printf(" ");
  bargraph(meter_state[6]&SEGMENT_J,meter_state[6]&SEGMENT_K);
  for(i=0;i<6;i++) {
    red_or_grey(meter_state[i]&SEGMENT_A);
    printf(" ___  ");
  }
  bargraph(meter_state[7]&SEGMENT_J,meter_state[7]&SEGMENT_K);
  printf("\n");

  /* fourth line */
  printf(" ");
  bargraph(meter_state[6]&SEGMENT_G,meter_state[6]&SEGMENT_H);
  for(i=0;i<6;i++) {
    red_or_grey(meter_state[i]&SEGMENT_F);
    printf("|");
    red_or_grey(meter_state[i]&SEGMENT_P);
    printf("\\");
    red_or_grey(meter_state[i]&SEGMENT_G);
    printf("|");
    red_or_grey(meter_state[i]&SEGMENT_H);
    printf("/");
    red_or_grey(meter_state[i]&SEGMENT_B);
    printf("| ");
  }
  bargraph(meter_state[7]&SEGMENT_G,meter_state[7]&SEGMENT_H);
  printf("\n");

  /* fifth line */
  printf(" ");
  bargraph(meter_state[6]&SEGMENT_E,meter_state[6]&SEGMENT_F);
  for(i=0;i<6;i++) {
    red_or_grey(meter_state[i]&SEGMENT_N);
    printf(" - ");
    red_or_grey(meter_state[i]&SEGMENT_J);
    printf("-  ");
  }
  bargraph(meter_state[7]&SEGMENT_E,meter_state[7]&SEGMENT_F);
  printf("\n");

  /* sixth line */
  printf(" ");
  bargraph(meter_state[6]&SEGMENT_C,meter_state[6]&SEGMENT_D);
  for(i=0;i<6;i++) {
    red_or_grey(meter_state[i]&SEGMENT_E);
    printf("|");
    red_or_grey(meter_state[i]&SEGMENT_M);
    printf("/");
    red_or_grey(meter_state[i]&SEGMENT_L);
    printf("|");
    red_or_grey(meter_state[i]&SEGMENT_K);
    printf("\\");
    red_or_grey(meter_state[i]&SEGMENT_C);
    printf("| ");
  }
  bargraph(meter_state[7]&SEGMENT_C,meter_state[7]&SEGMENT_D);
  printf("\n");

  /* seventh line */
  printf(" ");
  bargraph(meter_state[6]&SEGMENT_A,meter_state[6]&SEGMENT_B);
  for(i=0;i<6;i++) {
    red_or_grey(meter_state[i]&SEGMENT_D);
    printf(" --- ");
    red_or_grey(meter_state[i]&SEGMENT_DP);
    printf(".");
  }
  bargraph(meter_state[7]&SEGMENT_A,meter_state[7]&SEGMENT_B);
  printf("\n");

  /* restore default colors */
  printf("\033[0;39;49m\n");


  return;
}

#if 0

int main(int argc, char **argv) {

  char i2c_buffer[256];

  reset_display();
  print_meter();

  emulate_i2c("\x70\x00\x46\x00\x00\x00\x00",7);
  print_meter();

  emulate_i2c("\x72\x00\x46\x00\x00\x00\x00",7);
  print_meter();

  emulate_i2c("\x74\x00\x46\x00\x00\x00\x00",7);
  print_meter();

  emulate_i2c("\x76\x00\x46\x00\x00\x00\x00",7);
  print_meter();

  emulate_i2c("\x70\x01\xff\xff\xff\xff",6);
  emulate_i2c("\x72\x01\xff\xff\xff\xff",6);
  emulate_i2c("\x74\x01\xff\xff\xff\xff",6);
  emulate_i2c("\x76\x01\xff\xff\xff\xff",6);
  print_meter();


 
  meter_state[0]=ascii_lookup['H']|SEGMENT_EX;
  meter_state[1]=ascii_lookup['E']|SEGMENT_EX;
  meter_state[2]=ascii_lookup['L']|SEGMENT_EX;
  meter_state[3]=ascii_lookup['L']|SEGMENT_EX;
  meter_state[4]=ascii_lookup['O']|SEGMENT_EX;
  meter_state[5]=ascii_lookup['!']|SEGMENT_EX;
  meter_state[6]=0xaa55;
  meter_state[7]=0xf0f0;

  print_meter();

  return 0;
}

#endif
