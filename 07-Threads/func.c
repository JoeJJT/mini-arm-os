#include <stdio.h>
#include "func.h"

int strcmp(const char *s1, const char *s2) {
	while(*s1 != '\0' && *s1 == *s2) {
		*s1++;*s2++;
	}
	if(*s1 == '\0' && *s2 == '\0') return 1;
	return 0;
}

int strlen(const char *s) {
	int len=0;
	while(s[len]) {
		len++;
	}
	return len;
}

int atoi(const char *s) {
	int sum=0;
	int i;
	for(i = 0; s[i] != '\0' ; i++) {
		sum = sum*10+s[i] - '0';
	}
	return sum;
}

void itoa(int ,char *) {

}

void reverse(char *s) {

}
