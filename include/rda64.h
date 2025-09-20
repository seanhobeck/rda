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

/*! @uses rda_dynl_t */
#include "dynl.h"

// @note a structure for a simplified, decompiled instruction in amd64/x86_64.
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


/// FUTURE: perhaps we start adding support for x87 fpu, sse (not 2), avx512?
///     finish most of amd64/x86_64 and then move onto arm64.


// @note a structure for a simplified, disassembled function in amd64/x86_64.
typedef struct {
    rda_dynl_t* ilist; // a list of decoded instructions in a function.
    unsigned char* bytes; // the actual bytes processed (trimmed).
    size_t address, length; // the address (unsigned long) and the length of bytes processed.
} rda_fun_amd64_t;

/**
 * @brief disassemble a function in memory at <address>.
 *
 * @param address the address in memory to start reading from.
 * @return a pointer to an allocated structure containing the information
 *  about the disassembled function in amd64.
 */
rda_fun_amd64_t*
rda_disassemble_amd64(void* address);
#endif //LRDA64_H
