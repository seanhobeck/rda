/**
 *
 *	@author Sean Hobeck
 *	@date 15/09/2025
 *
 */
#include "dis.h"

/*! @uses calloc */
#include <stdlib.h>

/*! @uses memcpy */
#include <string.h>

/*! @uses printf */
#include <stdio.h>

/**
 * @brief analyze bytes using the amd64 architecture,
 *  given some bytes and a size.
 *
 * @param code the bytes in memory to analyze.
 * @param size the size of bytes in memory to analyze.
 * @return
 */
rda_di_t*
rda_analyze_iamd64(const unsigned char* code, size_t size) {
	// create our disassembled instruction.
	rda_di_t* di = calloc(1, sizeof *di);
	di->bytes = code;
	di->max_size = size;

	// now we iterate through each instruction in the amd64 table
	//	to see if we can find something that looks like the opcodes we are given.
	for (size_t i = 0; i < RDA_AMD64_TABLE_SZ; i++) {
		const i_amd64_t inst = table_amd64[i];

		// opcode match memcmp
		if (memcmp(code, inst.opcode, inst.olen) == 0) {
			int match = 0;

			if (inst.plus_reg == 1) {
				// check all but last byte of opcode
				if (inst.olen == 1) {
					if ((code[0] & 0xF8) == inst.opcode[0]) match = 1;
				} else {
					if (memcmp(code, inst.opcode, inst.olen - 1) == 0 &&
						(code[inst.olen-1] & 0xF8) == inst.opcode[inst.olen-1]) {
						match = 1;
						}
				}
			} else {
				if (memcmp(code, inst.opcode, inst.olen) == 0) match = 1;
			}

			if (!match) continue;

			// --- Step 2: length starts at opcode length ---
			int len = inst.olen;

			// --- Step 3: decode ModR/M if needed ---
			if (inst.modrm) {
				if (len >= size) return 0x0;
				unsigned char modrm = code[len++];
				unsigned char mod = (modrm >> 6) & 3;
				unsigned char reg = (modrm >> 3) & 7;
				unsigned char rm  = (modrm & 7);

				// check /digit encoding
				if (inst.modrm_reg != -1 && inst.modrm_reg != reg) continue;

				// handle SIB/displacement
				if (mod != 3 && rm == 4) len++;         // SIB byte
				if (mod == 1) len += 1;                 // disp8
				else if (mod == 2) len += 4;            // disp32
				else if (mod == 0 && rm == 5) len += 4; // disp32 special
			}

			// --- Step 4: adjust for immediate ---
			if (inst.ilen > 0) {
				len += inst.ilen;
			} else if (inst.ilen == -1) {
				// some instructions have operand-size dependent imm
				if (inst.size == 16) len += 2;
				else if (inst.size == 32) len += 4;
				else if (inst.size == 64) len += 4; // often sign-extended imm32
			}

			if (len > size) return 0x0;

			// printf, and memcpy the actual
			printf("decoded: %s (len=%d)\n", inst.mnemonic, len);
			di->iptr = calloc(1, sizeof(i_amd64_t));
			*di->iptr = inst;
			di->length = len;
			return di;
		}
	}
	return 0x0;
};