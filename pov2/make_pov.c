/* make_pov.c  --  by Vince Weaver -- http://www.deater.net/weave/vmwprod */
/*                                 -- 16 June 2006 - Under GPL2 license   */
/* Make pov include file from an ASCII text layout                        */
/* compile with:                                                          */
/*       gcc -O2 -Wall -o make_pov make_pov.c                             */
/*                                                                        */
/* Run with something like:                                               */
/*     ./make_pov < test > test.h                                         */
/* Where test is 8 lines of plain text.  A space (ascii 32) will become   */
/*   a zero, any non-space character will become a 1.  Make sure to get   */
/*   rid of tailing whitespace you don't need, as the output is           */
/*   auto-sized.                                                          */

#include <stdio.h>
#include <string.h> /* strlen() */
#include <stdlib.h> /* realloc() */

void print_binary(unsigned char byte) {
   int i;
   
   for(i=7;i>=0;i--) {
      if (byte & 1<<i) printf("1");
      else printf("0");
   }
}

int main(int argc, char **argv) {
 
   char buffer[BUFSIZ];
   
   int i,j,length=0,max_length=0;
   unsigned char *design=NULL;
   
   for(i=0;i<8;i++) {
      fgets(buffer,BUFSIZ,stdin);
      length=strlen(buffer)-1;
      if (length>max_length) {
	 max_length=length;
	 design=realloc(design,max_length*sizeof(char));
      }
      for(j=0;j<length;j++) {
	 if(buffer[j]!=' ') {
	   design[j]|=1<<i;
	 }
      }
   }
   
   printf("const static int image[] = {\n");
   for(i=0;i<max_length;i++) {
      printf("\tB8(");
      print_binary(design[i]);
      printf("),\n");
   }
   printf("};\n");
   return 0;
}
