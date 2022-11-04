#include "util.h"
#include <stdio.h>
#include <stdlib.h>

int from_hex4(int n) {
	return ((n & 0x000000ff) << 24) | ((n & 0x0000ff00) << 8)
		| ((n & 0x00ff0000) >> 8) | ((n & 0xff000000) >> 24);
}

char* new_str(int len) {
	char* str = malloc((len + 1) * sizeof(char));
	if (!str) {
		printf("alloc type error.\n");
		return NULL;
	}
	str[len] = '\0';
	return str;
}