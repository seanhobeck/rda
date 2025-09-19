/**
 *	@author Sean Hobeck
 *	@date 19/09/2025
 *
 *	@file rda64.h
 */
#ifndef LRDA64_H
#define LRDA64_H

/*! @uses size_t */
#include <stddef.h>

/*! @uses bool, true, false */
#include <stdbool.h>

/*! @uses amd64_prefixes_t, amd64_int_t. */
#include "asmx64.h"

// @note a structure for a simplified, decompiled instruction in amd64.
typedef struct {
    amd64_int_t instruction;        // instruction information, see amd64.h
    const unsigned char* bytes;     // raw bytes read from memory.
    size_t length, prefix_count;    // total byte length, and prefix count.
    int rex_byte;                   // the rex byte value (0 if not found).
    bool valid;                     // if the instruction is valid.
} rda_int_amd64_t;

/**
 * @brief decode a single instruction in memory at <bytes>.
 *
 * @param bytes the bytes in memory to be decoded.
 * @param size the size of <bytes> (at the most 4).
 * @return a pointer to an allocated structure containing the information
 *  about the decoded instruction in amd64.
 */
rda_int_amd64_t*
rda_decode_single_amd64(const unsigned char* bytes, size_t size);

/**
 * @brief get the instruction type of a decoded instruction.
 *
 * @param inst a decoded instruction from librda.
 * @return the category/type of the instruction.
 */
amd64_int_type_t
rda_get_type(const rda_int_amd64_t* inst);

/// TODO: add rda_fun_amd64_t for function analysis containing a rda_dynl_t of
///     instructions within the function.
///
/// FUTURE: perhaps we start adding support for x87 fpu, sse (not 2), avx512?
///     finish most of amd64/x86_64 and then move onto arm64 (if i have the will power).
#endif //LRDA64_H
