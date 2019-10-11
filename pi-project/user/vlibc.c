#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>

#include "../syscalls.h"
#include "vlibc.h"


uint32_t write(int fd, const void *buf, size_t count) {

	uint32_t result;

	result=syscall3(SYSCALL_WRITE,fd,(uint32_t)buf,count);

	return result;

}

int putchar(int ch) {

	uint32_t result;

	char buffer[1];

	buffer[0]=ch;

	result=syscall3(SYSCALL_WRITE,STDOUT,(uint32_t)buffer,1);

	return result;

}

int getchar(void) {

	uint32_t result;
	int32_t ch=0;
	char buffer[1];

	while(1) {
		/* Nonblocking for now */
		result=syscall3(SYSCALL_READ,STDIN,(uint32_t)buffer,1);

		ch=buffer[0];

		if (ch>0) break;

	}
	return ch;

}


#define MAX_PRINT_SIZE 256

int printf(char *string,...) {

	va_list ap;

	char buffer[MAX_PRINT_SIZE];
	char int_buffer[10];
	int int_pointer=0;

	int buffer_pointer=0;
	int i;
	int x;

	va_start(ap, string);

	while(1) {
		if (*string==0) break;

		if (*string=='%') {
			string++;
			if (*string=='d') {
				string++;
				x=va_arg(ap, int);
				int_pointer=9;
				do {
					int_buffer[int_pointer]=(x%10)+'0';
					int_pointer--;
					x/=10;
				} while(x!=0);
				for(i=int_pointer+1;i<10;i++) {
					buffer[buffer_pointer]=int_buffer[i];
					buffer_pointer++;
				}

			}
			else if (*string=='x') {
				string++;
				x=va_arg(ap, int);
				int_pointer=9;
				do {
					if ((x%16)<10) {
						int_buffer[int_pointer]=(x%16)+'0';
					}
					else {
						int_buffer[int_pointer]=(x%16)-10+'a';
					}
					int_pointer--;
					x/=16;
				} while(x!=0);
				for(i=int_pointer+1;i<10;i++) {
					buffer[buffer_pointer]=int_buffer[i];
					buffer_pointer++;
				}
			}
		}
		else {
			buffer[buffer_pointer]=*string;
			buffer_pointer++;
			string++;
		}
		if (buffer_pointer==MAX_PRINT_SIZE-1) break;
	}

	va_end(ap);

	write(STDOUT,buffer,buffer_pointer);

	return buffer_pointer;
}
