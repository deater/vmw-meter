#include <stddef.h>
#include <stdint.h>
#include "../syscalls.h"
#include "vlibc.h"


static int parse_input(char *string) {

	int result=0;
	int ticks;

	if (!strncmp(string,"led",sizeof(string))) {
		printf("VMW: LED\n");
	}
	else if (!strncmp(string,"time",sizeof(string))) {
		syscall1(SYSCALL_TIME,(long)&ticks);
		printf("Time: %d\n",ticks);
	}
	else if (!strncmp(string,"gradient",sizeof(string))) {
		syscall1(SYSCALL_GRADIENT,(long)0);
	}
	else if (!strncmp(string,"print",sizeof(string))) {
		printf("Hello World!\n");
	}
	else if (!strncmp(string,"help",sizeof(string))) {
		printf("One of following: led,time,print,help\n");
	}
	else {
		printf("\nUnknown commmand!\n");
	}

	return result;
}


uint32_t shell(void) {

	char input_string[256];
	uint32_t input_pointer,ch;

	/* Enter our "shell" */
	printf("\nReady!\n");

	while (1) {
		input_pointer=0;
		printf("] ");

		while(1) {
			ch=getchar();
			if ((ch=='\n') || (ch=='\r')) {
				input_string[input_pointer]=0;
				printf("\n");
				parse_input(input_string);
				break;
			}

			input_string[input_pointer]=ch;
			input_pointer++;
			putchar(ch);
		}
	}
}
