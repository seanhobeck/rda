#include "../include/asm/amd64.h"
#include <stdio.h>

int main(int argc, const char * argv[]) {
	printf("%p\n", &table_amd64);
	return 0;
}