/**
 *	@author Sean Hobeck
 *	@date 23/10/2025
 *
 *	@file rda64.c
 */
#include "rda64.h"

/*! @uses calloc */
#include <stdlib.h>

/*! @uses memcpy, memcmp */
#include <string.h>

/*! @uses rda_internal */
#include "lib.h"

/*! @uses internal_simd_table */
#include "simdx64.h"

/**
 * @brief parse the prefixes with a max of 5 prefixes.
 *
 * @param bytes the bytes to be parsed for any prefixes.
 * @param size the count of <bytes> (at least 1).
 * @param rex_ptr pointer to a rexw prefix in an instruction.
 * @return a prefix count in <code> (minimum 0).
 */
rda_internal size_t
parse_prefixes(const unsigned char* bytes, size_t size, unsigned char* rex_ptr) {
    // set a count and then set the rex value to be 0.
    size_t prefix_count = 0;
    *rex_ptr = 0;

    // read up until <size> bytes but also no more than 5 prefixes.
    while (prefix_count < size && prefix_count < 5) {
        // read the byte at the prefix_count index, and then compare it
        //  with the table to see if it returns a prefix_type of != 0.
        unsigned char byte = bytes[prefix_count];
        unsigned char prefix_type = internal_prefix_table[byte];

        // not a prefix
        if (prefix_type == 0)
            break;

        // rex.? prefix
        if (prefix_type == 2) {
            *rex_ptr = byte;
            prefix_count++;
            break;  // rex must be last prefix that we read.
        }

        // legacy prefix.
        prefix_count++;
    }
    return prefix_count;
};

/**
 * @brief check if the 4 bytes provided are actually a f3 prefix,
 *  and not something like endbr32/64.
 *
 * @param bytes the bytes to be checked for an actual f3 prefix.
 * @param size the count of <bytes> (at least 1).
 * @return 1 if the f3 is a prefix, 0 if it is endbr32/64.
 */
rda_internal size_t
is_f3_prefix_context(const unsigned char* bytes, size_t size) {
    if (size < 4) return 1; // assume prefix if not enough bytes

    // check for endbr32/64 (f3 0f 1e fa/fb)
    if (bytes[0] == 0xf3 && bytes[1] == 0x0f && bytes[2] == 0x1e &&
        (bytes[3] == 0xfa || bytes[3] == 0xfb)) {
        return 0; // not a prefix.
    }
    return 1; // is a rep prefix.
};

/**
 * @brief getting the length of a modr/m byte (disp8, disp32, rip-rel disp32).
 *
 * @param modrm the modr/m byte to be analyzed.
 * @return the length of the modr/m byte.
 */
rda_internal size_t
get_modrm_length(unsigned char modrm) {
    /**
     *  breaking down mod rm as a byte, visually,
     *
     *            1 byte (aka. modr/m)
     *      v--------------------------------v
     *      .________________________________.
     *      |  mod   |    reg    |    r/m    |
     *      | 2 bits |   3 bits  |   3 bits  |
     *      |________|___________|___________|
     *
     * 'mod' is the addressing mode, ie. register vs. memory + displacement (offset),
     * 'reg' is the selected register (if chosen, or a opcode extenstion),
     * 'r/m' is combined with 'mod' to pick the base register or addressing form.
     */
    unsigned char mod = (modrm >> 6) & 3;
    unsigned char rm = modrm & 7;
    if (mod == 3) {
        return 1; // just 1 byte.
    }

    // check for sib-byte (0b100) or 4.
    size_t length = 1;
    if (rm == 4) {
        length += 1;
    }

    // add displacement
    if (mod == 1) {
        length += 1; // disp8
    } else if ((mod == 0 && rm == 5) || mod == 2) {
        length += 4; // rip-relative disp32 or disp32
    }
    return length;
};

/**
 * @brief match, compare, and calculate the bytes for the rda_int_t instruction,
 *  returning the length if they match, and -1 otherwise.
 *
 * @param bytes the bytes to be matched.
 * @param available the number of available bytes.
 * @param inst the amd64 instruction to compare against.
 * @param prefix_len the size of the prefix to compare and calculate against.
 * @return the length of bytes read, -1 if they do not match.
 */
rda_internal int
match_and_calc_length(const unsigned char* bytes, size_t available,
    const rda_int_t* inst, size_t prefix_len) {
    // grabbing a pointer to the current byte from the code + prefix_len.
    const unsigned char* byte_ptr = bytes + prefix_len;
    size_t remaining = available - prefix_len;
    if (remaining < inst->opcode_length) return -1; // not enough bytes for opcode

    // quick opcode matching
    if (inst->plus_reg) {
        // +rd encoding - mask lower 3 bits of last opcode byte
        if (inst->opcode_length == 1) {
            // if rex.w
            if (prefix_len == 1 && inst->opcode_size == 64) {
                if (bytes[0] != 0x48)
                    return -1;
                if (bytes[1] != inst->bytes[1])
                    return -1;
            }
            else if ((byte_ptr[0] & 0xf8) != (inst->bytes[0] & 0xf8))
                return -1;
        }
        else {
            if (memcmp(byte_ptr, inst->bytes, inst->opcode_length - 1) != 0) return -1;
            if ((byte_ptr[inst->opcode_length - 1] & 0xf8) != \
                (inst->bytes[inst->opcode_length - 1] & 0xf8)) {
                return -1;
            }
        }
    } else {
        // exact match
        if (memcmp(byte_ptr, inst->bytes, inst->opcode_length) != 0)
            return -1;
    }

    // set the bare minimum length to be equal to
    int length = (int) prefix_len + inst->opcode_length;

    // handle modr/m byte
    if (inst->modrm) {
        if (length >= available) return -1;
        unsigned char modrm = bytes[length];

        // check /digit encoding
        if (inst->modrm_reg != -1) {
            unsigned char reg = (modrm >> 3) & 7;
            if (reg != inst->modrm_reg) return -1;
        }

        // calculate the modrm length, and if it is more than we have
        //  available, then we simply return -1.
        size_t modrm_len = get_modrm_length(modrm);
        if (length + modrm_len > available) return -1;
        length += (int) modrm_len;
    }

    // handle immediate values
    if (inst->instruction_length > 0) {
        length += inst->instruction_length;
    } else if (inst->instruction_length == -1) {
        // operand-size dependent
        if (inst->opcode_size == 64) {
            length += 4; // usually sign-extended 32-bit in 64-bit mode
        } else if (inst->opcode_size == 32) {
            length += 4;
        } else if (inst->opcode_size == 16) {
            length += 2;
        } else {
            length += 4; // default
        }
    }
    return (length <= available) ? length : -1;
};

/**
 * @brief decode a single instruction in memory.
 *
 * @param bytes the bytes in memory to be decoded.
 * @param size the size of <bytes> (at the most 4).
 * @return a pointer to an allocated structure containing the information
 *  about the decoded instruction in amd64.
 */
rda_dec_int_t*
rda_decode_single64(const unsigned char* bytes, size_t size) {
    // allocate a instruction and then return it.
    rda_dec_int_t* result = calloc(1u, sizeof *result);
    if (!bytes || size == 0) {
        result->valid = false;
        return result; // we want to fail silently, this is a shared object after all.
    }

    // special handling for 0xf3 prefix vs. endbr32/64 instructions,
    size_t prefix_length = 0;
    unsigned char rex = 0;
    if (bytes[0] == 0xf3 && !is_f3_prefix_context(bytes, size)) {
        // this 0xf3 is part of endbr32/64, NOT a prefix.
        prefix_length = 0;
    } else {
        // normal prefix parsing.
        prefix_length = parse_prefixes(bytes, size, &rex);
    }

    // this should never happen.
    if (prefix_length >= size) {
        return result; // only prefixes, no instruction
    }

    // try simd instruction table first,
    rda_context_t ctx = rda_get_context();
    if (ctx.use_simd) {
        for (size_t i = 0; i < sizeof(internal_simd_table) / sizeof(rda_int_t); i++) {
            // iterate through each instruction and see if anything remotely matches.
            const rda_int_t* inst = &internal_simd_table[i];
            int length = match_and_calc_length(bytes, size, inst, prefix_length);
            if (length > 0) {
                // found a simd match!
                result->instruction = *inst;
                result->bytes = bytes;
                result->length = length;
                result->prefix_count = prefix_length;
                result->rex_byte = rex;
                result->valid = true;
                return result;
            }
        }
    }

    // we then try the main instruction table.
    for (size_t i = 0; i < sizeof(internal_table) / sizeof(rda_int_t); i++) {
        // iterate through each instruction and see if anything remotely matches.
        const rda_int_t* inst = &internal_table[i];
        int length = match_and_calc_length(bytes, size, inst, prefix_length);
        if (length > 0) {
            // found a match!
            result->instruction = *inst;
            result->bytes = bytes;
            result->length = length;
            result->prefix_count = prefix_length;
            result->rex_byte = rex;
            result->valid = true;
            return result;
        }
    }

    // no match found, this instruction is 'unrecognized'.
    result->bytes = bytes;
    result->length = 1; // skip one byte
    result->valid = false;
    return result;
};

/**
 * @brief get the instruction type of decoded instruction.
 *
 * @param inst a decoded instruction.
 * @return the category/type of the instruction.
 */
rda_int_ty_t
rda_get_type(const rda_dec_int_t* inst) {
    // this should not happen.
    if (!inst) return RDA_INST_TY_INVALID;
    return inst->instruction.type;
};

/**
 * @brief disassemble a function in memory at an address.
 *
 * @param address the address in memory to start reading from.
 * @return a pointer to an allocated structure containing the information
 *  about the disassembled function in amd64.
 */
rda_dec_fun_t*
rda_disassemble64(void* address) {
    // allocate the structure.
    rda_dec_fun_t* function = calloc(1u, sizeof *function);
    function->address = (size_t) address;
    function->list = rda_dynl_create(sizeof(rda_dec_int_t));

    // we then iterate.
    size_t offset = 0;
    unsigned char* bytes = address;
    while (1) {
        // decode instruction at current offset
        rda_dec_int_t* inst = rda_decode_single64(bytes + offset, 15);
        if (!inst) break; // decoder failed badly

        // add to a function instruction list.
        rda_dynl_push(function->list, inst);

        // inc offset
        offset += inst->length;

        // invalid instruction, break.
        if (!inst->valid)
            break;

        // is this a return instruction?
        if (inst->instruction.type == RDA_INST_TY_CONTROL && \
            strncmp(inst->instruction.mnemonic, "ret", 3) == 0)
            break;
    }

    // record total size of bytes consumed
    function->bytes = calloc(1u, offset);
    memcpy(function->bytes, bytes, offset);
    return function;
};

/**
 * @brief get the instruction at index within a function.
 *
 * @param function a decoded function from memory.
 * @param index the index within the list of instructions.
 * @return instruction within the function at <index> or 0x0 if not found.
 */
rda_dec_int_t*
rda_get_instruction_at(rda_dec_fun_t* function, size_t index) {
    return rda_dynl_get(function->list, index);
};