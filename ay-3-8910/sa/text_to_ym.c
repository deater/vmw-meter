#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <arpa/inet.h>

struct ym_header {
	char id[4];
	char check[8];
	uint32_t vbl;
	uint32_t song_attr;
	uint32_t digidrum;
	uint32_t external_frequency;
	uint16_t player_frequency;
	uint32_t loop;
	uint16_t additional_data;
}  __attribute__((packed)) our_header;

int main(int argc, char **argv) {

	char string[BUFSIZ];
	char *result;
	char *outfile;
	FILE *fff;
	int frames=0,digidrums=0,external_frequency=1000000;
	int frequency=50;
	int loop=0;

	outfile=strdup("out.ym");

	fff=fopen(outfile,"w");
	if (fff==NULL) {
		fprintf(stderr,"Couldn't open %s\n",outfile);
		return -1;
	}

	while(1) {
		result=fgets(string,BUFSIZ,stdin);
		if (result==NULL) break;

		/* skip comments */
		if (string[0]=='\'') continue;
		if (string[0]=='-') continue;

		printf("%s",string);
	}

	rewind(fff);

	strcpy(our_header.id,"YM5!");
	strcpy(our_header.check,"LeOnArD!");
	our_header.vbl=htonl(frames);
	our_header.digidrum=htonl(digidrums);
	our_header.external_frequency=htonl(external_frequency);
	our_header.player_frequency=htonl(frequency);
	our_header.loop=htonl(loop);
	our_header.additional_data=htonl(0);


	fwrite(&our_header,sizeof(struct ym_header),1,fff);

	fclose(fff);

	return 0;
}
