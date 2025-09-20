/**
 * @author Sean Hobeck
 * @date 19/09/2025
 *
 * @file main.c
 */


#include <stddef.h>
#include <stdio.h>

#include "lib.h"
#include "rda64.h"


int some_function(int a, int b) {
	int i = b;
	for (size_t j = 0; j < a; j++)
		i += j;
	return a * i;
}

// entry point for testing.
int main(int argc, char** argv) {
	rda_begin(0x0);
	rda_fun_amd64_t* function = rda_disassemble_amd64(&rda_disassemble_amd64);
	for (size_t i = 0; i < function->ilist->length; i++) {
		printf("%s\n", ((rda_int_amd64_t*)rda_dynl_get(function->ilist, i))->instruction.mnemonic);
	}
	return 0;
};