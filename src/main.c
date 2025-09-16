#include "../include/amd64.h"
#include <stdio.h>

#include "dis.h"
#include "dynl.h"

void some_function () {
	int j = 1;
	for (int i = 0; i < 10; i++) {
		j *= 2;
	}
};

int main(int argc, const char * argv[]) {
	unsigned char *p = &some_function;
	_int_dynl_t* list = _int_dynl_create(sizeof(rda_di_t*));
	size_t n = 128;

	size_t offset = 0;
	while (offset < n) {
		rda_di_t* di = rda_analyze_iamd64(p + offset, n - offset);
		if (di == 0x0) {
			offset++;
			printf("skipped\n");
			continue;
		}
		_int_dynl_push(list, di);
		if (di->length <= 0) break;
		offset += di->length;
	}
	return 0;
}