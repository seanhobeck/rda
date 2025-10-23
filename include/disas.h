/**
 *	@author Sean Hobeck
 *	@date 23/10/2025
 *
 *	@file disas.h
 */
#ifndef LRDA_DISAS_H
#define LRDA_DISAS_H

/*! @uses size_t */
#include <stddef.h>

/*! @uses bool, true, false */
#include <stdbool.h>

/*! @uses rda_int_t, internal_table. */
#include "asmx64.h"

/*! @uses rda_dynl_t, rda_dynl_get */
#include "dynl.h"

// @note a structure for a simplified, decompiled instruction in amd64/x86_64.
typedef struct {
    rda_int_t instruction;          // instruction information, see asmx64.h
    const unsigned char* bytes;     // raw bytes read from memory.
    size_t length, prefix_count;    // total byte length and prefix count.
    int rex_byte, vex_encoding;     // the rex byte value (0 = ?), and vex encoding (0 = ?, 1 = vex, 2 = evex).
    bool valid;                     // if the instruction is valid.
} rda_dec_int_t;

/**
 * @brief decode a single instruction in memory.
 *
 * @param bytes the bytes in memory to be decoded.
 * @param size the size of <bytes> (at the most 4).
 * @return a pointer to an allocated structure containing the information
 *  about the decoded instruction in amd64.
 */
rda_dec_int_t*
rda_decode_single64(const unsigned char* bytes, size_t size);

/**
 * @brief get the instruction type of decoded instruction.
 *
 * @param inst a decoded instruction.
 * @return the category/type of the instruction.
 */
rda_int_ty_t
rda_get_type(const rda_dec_int_t* inst);

// @note a structure for a simplified, disassembled function in amd64/x86_64.
typedef struct {
    rda_dynl_t* list; // a list of decoded instructions in a function.
    unsigned char* bytes; // the actual bytes processed (trimmed).
    size_t address, length; // the address (unsigned long) and the length of bytes processed.
} rda_dec_fun_t;

/**
 * @brief disassemble a function in memory at an address.
 *
 * @param address the address in memory to start reading from.
 * @return a pointer to an allocated structure containing the information
 *  about the disassembled function in amd64.
 */
rda_dec_fun_t*
rda_disassemble64(void* address);

/**
 * @brief get the instruction at index within a function.
 *
 * @param function a decoded function from memory.
 * @param index the index within the list of instructions.
 * @return instruction within the function at <index> or 0x0 if not found.
 */
rda_dec_int_t*
rda_get_instruction_at(rda_dec_fun_t* function, size_t index);
#endif //LRDA_DISAS_H
