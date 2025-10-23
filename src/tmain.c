/**
 * @author Sean Hobeck
 * @date 23/10/2025
 *
 * @file tmain.c
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

#include <emmintrin.h>  // sse2 intrinsics

void example_sse2(const int *a, const int *b, int *out) {
	__m128i va = _mm_loadu_si128((const __m128i*)a);
	__m128i vb = _mm_loadu_si128((const __m128i*)b);
	__m128i vc = _mm_add_epi32(va, vb); // paddd
	_mm_storeu_si128((__m128i*)out, vc);
}

#include <immintrin.h>  // avx512 intrinsics

void example_avx512(const float *a, const float *b, float *out) {
	__m512 va = _mm512_loadu_ps(a);
	__m512 vb = _mm512_loadu_ps(b);
	__m512 vc = _mm512_add_ps(va, vb); // vaddps
	_mm512_storeu_ps(out, vc);
}

float example_regular_floating_point(float a, float b) {
	return a*a + b*7.2f;
}

double example_regular_floating_point2(double a, float b) {
	return a*a + b/ 7.2;
}

int other_function(int x, size_t z) {
	size_t y = z*z + x;
	printf("%zu\n", y+111);
	return (int)(y % 7);
}

// entry point for testing.
int main(int argc, char** argv) {
	rda_context_t ctx = (rda_context_t) {
		.verbose = true,
		.use_simd = true,
	};
	rda_begin(ctx);

	// disassemble some example functions.
	rda_dec_fun_t* function = rda_disassemble64(&other_function);
	for (size_t i = 0; i < function->list->length; i++) {
		printf("%s\n", rda_get_instruction_at(function, i)->instruction.mnemonic);
	}

	puts("\n\n");
	function = rda_disassemble64(&rda_dynl_create);
	for (size_t i = 0; i < function->list->length; i++) {
		printf("%s\n", rda_get_instruction_at(function, i)->instruction.mnemonic);
	}

	// disassemble example_sse2.
	// printf("\n\n");
	// function = rda_disassemble64(&example_sse2);
	// for (size_t i = 0; i < function->list->length; i++) {
	// 	printf("%s\n", ((rda_dec_int_t*)rda_dynl_get(function->list, i))->instruction.mnemonic);
	// }

	// // disassemble example_avx512.
	// printf("\n\n");
	// function = rda_disassemble_amd64(&example_avx512);
	// for (size_t i = 0; i < function->ilist->length; i++) {
	// 	printf("%s\n", ((rda_int_amd64_t*)rda_dynl_get(function->ilist, i))->instruction.mnemonic);
	// }

	// // disassemble example_regular_floating_point
	// printf("\n\n");
	// function = rda_disassemble_amd64(&example_regular_floating_point);
	// for (size_t i = 0; i < function->ilist->length; i++) {
	// 	printf("%s\n", ((rda_int_amd64_t*)rda_dynl_get(function->ilist, i))->instruction.mnemonic);
	// }

	// disassemble example_regular_floating_point2.
	// printf("\n\n");
	// function = rda_disassemble_amd64(&example_regular_floating_point2);
	// for (size_t i = 0; i < function->ilist->length; i++) {
	// 	printf("%s\n", ((rda_int_amd64_t*)rda_dynl_get(function->ilist, i))->instruction.mnemonic);
	// }
	return 0;
};