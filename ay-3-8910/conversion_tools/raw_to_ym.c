/* Reads in raw text (made by dump_ym5 -r) and converts back to ym */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <math.h>


static int external_frequency=1000000;

struct ym_header {
	char id[4];				// 0  -> 4
	char check[8];				// 4  -> 12
	uint32_t vbl;				// 12 -> 16
	uint32_t song_attr;			// 16 -> 20
	uint16_t digidrum;			// 20 -> 22
	uint32_t external_frequency;		// 22 -> 26
	uint16_t player_frequency;		// 26 -> 28
	uint32_t loop;				// 28 -> 32
	uint16_t additional_data;		// 32 -> 34
}  __attribute__((packed)) our_header;






static int get_string(char *string, char *key, char *output, int strip_linefeed) {

	char *found;

	found=strstr(string,key);
	found=found+strlen(key);

	/* get rid of leading whitespace */
	while(1) {
		if ((*found==' ') || (*found=='\t')) found++;
		else break;
	}

	strcpy(output,found);

	/* remove trailing linefeed */
	if (strip_linefeed) output[strlen(output)-1]=0;

	return 0;

}





int main(int argc, char **argv) {

	char string[BUFSIZ];
	char *result;
	char ym_filename[BUFSIZ],*in_filename;
	char temp[BUFSIZ];
	FILE *ym_file,*in_file;
	int digidrums=0;
	int attributes=0;
	int irq=50,loop=0;
	int header_length=0;
	fpos_t save;
	int envelope_enable=1;
	int line=0,frames=0;

	char song_name[BUFSIZ];
	char author_name[BUFSIZ];
	char comments[BUFSIZ];
	char *comments_ptr=comments;


	/* Check command line arguments */
	if (argc<2) {
		printf("%s -- create a YM music file\n",argv[0]);
		printf("\n");
		printf("Usage:	%s INFILE [OUTFILE]\n\n",argv[0]);
		printf("\n");
		exit(1);
	}

	/* Open the input file */
	if (argv[1][0]=='-') {
		in_file=stdin;
	}
	else {
		in_filename=strdup(argv[1]);
		in_file=fopen(in_filename,"r");
		if (in_file==NULL) {
			fprintf(stderr,"Couldn't open %s\n",in_filename);
			return -1;
		}
	}

	/* Get the info for the header */
	while(1) {
		result=fgets(string,BUFSIZ,in_file);
		if (result==NULL) break;
		line++;

		if (strstr(string,"ENDHEADER")) break;

		if (strstr(string,"TITLE:")) {
			get_string(string,"TITLE:",song_name,1);
		}

		if (strstr(string,"AUTHOR:")) {
			get_string(string,"AUTHOR:",author_name,1);
		}

		if (strstr(string,"COMMENTS:")) {
			get_string(string,"COMMENTS:",comments_ptr,0);
			comments_ptr=&comments[strlen(comments)];
		}

		if (strstr(string,"FREQ:")) {
			get_string(string,"FREQ:",temp,1);
			external_frequency=atoi(temp);
		}

		if (strstr(string,"IRQ:")) {
			get_string(string,"IRQ:",temp,1);
			irq=atoi(temp);
		}

		if (strstr(string,"LOOP:")) {
			get_string(string,"LOOP:",temp,1);
			loop=atoi(temp);
		}

		if (strstr(string,"ENVELOPE:")) {
			get_string(string,"ENVELOPE:",temp,1);
			envelope_enable=atoi(temp);
		}
	}

	/* Open the output file */
	if (argc<3) {
		sprintf(ym_filename,"out.ym");
	}
	else {
		sprintf(ym_filename,"%s.ym",argv[2]);
	}

	ym_file=fopen(ym_filename,"w");
	if (ym_file==NULL) {
		fprintf(stderr,"Couldn't open %s\n",ym_filename);
		return -1;
	}

	/* Skip header, we'll fill in later */
	header_length=sizeof(struct ym_header)+
		strlen(song_name)+1+
		strlen(author_name)+1+
		strlen(comments)+1;

	fseek(ym_file, header_length, SEEK_SET);

	char fstr[32];
	char astr[32],bstr[32],cstr[32];
	char nstr[32],estr[32],mstr[32];
	char aamp[32],bamp[32],camp[32];
	unsigned char frame[16];
	int i,readlen;

	while(1) {
		result=fgets(string,BUFSIZ,in_file);
		if (result==NULL) break;
		if (string[0]==';') continue;
		readlen=sscanf(string,
			//F A  B  C  N  fE  M  AA AB AC E
			"%s %s %s %s %s %*s %s %s %s %s %s",
			fstr,astr,bstr,cstr,nstr,mstr,
			aamp,bamp,camp,estr);
		//printf("%d\n",readlen);
		if (readlen!=10) continue;
		frame[0]=(strtol(astr+2,NULL,16))&0xff;
		frame[1]=((strtol(astr+2,NULL,16))>>8)&0xf;
		frame[2]=(strtol(bstr+2,NULL,16))&0xff;
		frame[3]=((strtol(bstr+2,NULL,16))>>8)&0xf;
		frame[4]=(strtol(cstr+2,NULL,16))&0xff;
		frame[5]=((strtol(cstr+2,NULL,16))>>8)&0xf;
		frame[6]=((strtol(nstr+2,NULL,16)))&0xff;
		frame[7]=((strtol(mstr+2,NULL,16)))&0xff;
		frame[8]=((strtol(aamp+3,NULL,16)))&0xff;
		frame[9]=((strtol(bamp+3,NULL,16)))&0xff;
		frame[10]=((strtol(camp+3,NULL,16)))&0xff;
		frame[11]=((strtol(estr+2,NULL,16)))&0xff;
		frame[12]=((strtol(estr+2,NULL,16))>>8)&0xff;
		frame[13]=((strtol(estr+7,NULL,16)))&0xff;
		frame[14]=0;
		frame[15]=0;

		if (!envelope_enable) {
			frame[8]&=0xf;
			frame[9]&=0xf;
			frame[10]&=0xf;
			frame[11]=0;
			frame[12]=0;
			frame[13]=0xff;
		}

//		frame[7]|=0x2d;

		{
		printf("%04d: ",frames);
		for(i=0;i<14;i++) printf("%02x ",frame[i]);
		printf("\n");
		}

		fwrite(frame,1,16,ym_file);
		frames++;
	}


	fgetpos(ym_file,&save);

	rewind(ym_file);

	strncpy(our_header.id,"YM5!",4);
	strncpy(our_header.check,"LeOnArD!",8);
	our_header.vbl=htonl(frames);
	our_header.song_attr=htonl(attributes);
	our_header.digidrum=htonl(digidrums);
	our_header.external_frequency=htonl(external_frequency);
	our_header.player_frequency=htons(irq);
	our_header.loop=htonl(loop);
	our_header.additional_data=htons(0);

	fwrite(&our_header,sizeof(struct ym_header),1,ym_file);

	fprintf(ym_file,"%s%c",song_name,0);
	fprintf(ym_file,"%s%c",author_name,0);
	fprintf(ym_file,"%s%c",comments,0);

	fsetpos(ym_file,&save);

	fprintf(ym_file,"End!");

	fclose(ym_file);

	return 0;
}
