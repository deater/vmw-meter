#include <stdio.h>
#include <unistd.h>

int main(int argc, char **argv) {

	unsigned char triplet[3];
	int result;

	while(1) {
		result=read(0,triplet,3);
		if (result<0) break;

		printf("%02X %02X %02X\n",triplet[0],triplet[1],triplet[2]);

		if ((triplet[0]==0) && (triplet[1]==0) && (triplet[2]==0)) break;



	}


	return 0;
}
