/**
 *
 *	@author Sean Hobeck
 *	@date 15/09/2025
 *
 */
#ifndef LRDA_DIS_H
#define LRDA_DIS_H

/*! @uses size_t */
#include <stddef.h>

/*! @uses amd64 based assembly. */
#include "amd64.h"

/// @note a structure representing a disassembled instruction.
typedef struct {
	// a pointer to the amd64 instruction type.
	i_amd64_t* iptr;
	// a pointer to the actual bytes captured, max size, and length.
	unsigned char* bytes;
	size_t max_size, length;
} rda_di_t;

/**
 * @brief analyze bytes using the amd64 architecture,
 *  given some bytes and a size.
 *
 * @param code the bytes in memory to analyze.
 * @param size the size of bytes in memory to analyze.
 * @return
 */
rda_di_t*
rda_analyze_iamd64(const unsigned char* code, size_t size);
#endif //LRDA_DIS_H
