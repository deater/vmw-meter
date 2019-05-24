/* From vmwos */

#include <stdint.h>

#include "string.h"


int32_t strncmp(const char *s1, const char *s2, uint32_t n) {

	int i=0,r;

	while(1) {

		if (i==n) return 0;

		r=s1[i]-s2[i];
		if (r!=0) return r;

		i++;
	}

	return 0;
}

int32_t strlen(const char *s) {

	int length=0;

	while(s[length]) length++;

	return length;
}

int32_t atoi(char *string) {

	int result=0;
	char *ptr;

	ptr=string;

	while(*ptr!=0) {
		result*=10;
		result+=(*ptr)-'0';
		ptr++;
	}

	return result;
}

	/* Not optimized */
void *memset(void *s, int c, uint32_t n) {

	int i;
	char *ptr=(char *)s;

	for(i=0;i<n;i++) {
		ptr[i]=c;
	}

	return s;
}

	/* Not optimized */
int32_t memcmp(const void *s1, const void *s2, uint32_t n) {

	int i,result;
	char *c1,*c2;

	c1=(char *)s1;
	c2=(char *)s2;

	for(i=0;i<n;i++) {
		result=c1[i]-c2[i];
		if (result) return result;
	}

	return 0;
}
