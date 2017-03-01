#include <stdio.h>

int main() {
	union {
		short s;
		char c[sizeof(short)];
	} endian;

	endian.s = 0x0102;

	if (sizeof(endian.s) == 2) {
		if (endian.c[0] == 1 && endian.c[1] == 2)
			printf("big endian\n");
		else if (endian.c[0] == 2 && endian.c[1] == 1)
			printf("little endian\n");
		else 
			printf("unknown\n");
	}

	return 0;
}
