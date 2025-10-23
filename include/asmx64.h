/**
 *	@author Sean Hobeck
 *	@date 22/10/2025
 *
 *	@file asmx64.h
 */
#ifndef LRDA_ASMX64_H
#define LRDA_ASMX64_H

/// @note a byte type definition.
typedef unsigned char byte_t;

/**
 * @note a prefix detection lookup table, done via a designated
 *	initialized array. for more information about prefixes in
 *	amd64 assembly check: https://www.intel.com/content/www/us/en/developer/articles/technical/intel-sdm.html.
 */
static const byte_t internal_prefix_table[256] = {
	// segment overrides (es, cs, ss, ds, also branch taken and not taken).
	[0x26] = 1, [0x2e] = 1, [0x36] = 1, [0x3e] = 1,
	[0x64] = 1, [0x65] = 1, // segment overrides (fs, gs)
	[0x66] = 1, // operand-size override
	[0x67] = 1, // address-size override
	[0xf0] = 1, // lock
	[0xf2] = 1, [0xf3] = 1, // rep/repe/repz, repne/repnz

	// rex.? prefixes 0x40-0x4F
	[0x40] = 2, [0x41] = 2, [0x42] = 2, [0x43] = 2,
	[0x44] = 2, [0x45] = 2, [0x46] = 2, [0x47] = 2,
	[0x48] = 2, [0x49] = 2, [0x4a] = 2, [0x4b] = 2,
	[0x4c] = 2, [0x4d] = 2, [0x4e] = 2, [0x4f] = 2
};

/// @note an enum for the types of amd64/x86_64 instructions.
typedef enum {
	RDA_INST_TY_INVALID = 0x0,	// invalid instruction.
	RDA_INST_TY_DATA = 0x1,		// data movement instructions (mov, xchg, push, pop, lea, cmov, string ops as well).
	RDA_INST_TY_ARITH = 0x2,	// arithmetic instructions (add, sub, mul, div, imul, idiv, inc, dec, ...).
	RDA_INST_TY_LOGIC = 0x3,	// logic instructions (and, or, xor, test, not, neg, ...).
	RDA_INST_TY_CONTROL = 0x4,	// control-flow instructions (jmp, conditional jumps, calls, returns, loops and far calls).
	RDA_INST_TY_SYSTEM = 0x5,	// system instructions (int, syscall, hlt, ...).
	RDA_INST_TY_FLAG = 0x6,		// modify or read e/rflags registers (stc, clc, cmc, std, cld, lahf, sahf, pushf, popf).
	RDA_INST_TY_MISC = 0x7,		// miscellaneous instructions (nop, cpuid, etc ...).

	RDA_INST_TY_SSE = 0x8,		// sse instructions (movaps, addps, mulps, etc.).
	RDA_INST_TY_SSE2 = 0x9,		// sse2 instructions (movapd, addpd, mulpd, integer simd, etc.).
	RDA_INST_TY_SSE3 = 0xa,		// sse3 instructions (haddps, addsubpd, etc.)
	RDA_INST_TY_SSSE3 = 0xb,	// ssse3 instructions (pshufb, pabsb, etc.)
	RDA_INST_TY_SSE4_1 = 0xc,	// sse4.1 instructions (dpps, ptest, pmulld, etc.)
	RDA_INST_TY_SSE4_2 = 0xd,	// sse4.2 instructions (pcmpgtq, crc32, popcnt)
	RDA_INST_TY_AVX = 0xe,		// avx instructions (vmovaps, vaddps, etc.).
	RDA_INST_TY_AVX2 = 0xf,		// avx2 instructions (vpmovmskb, vpermq, etc.).
	RDA_INST_TY_AVX512 = 0x10,	// avx512 instructions (vmovaps, vaddps with evex encoding).
} rda_int_ty_t;

/// @note an amd64/x86_64 representation for an instruction.
typedef struct __attribute__((packed)) {
	const char* mnemonic; 			// mnemonic for the instruction.
	byte_t bytes[5];				// 4 starting bytes.
	int opcode_length, instruction_length, opcode_size, modrm;
	// ^ opcode/immediate length, opcode size, and the modr/m byte.
	int plus_reg, modrm_reg;		// +1 if using modr/m or +rd encoding respectively.
	rda_int_ty_t type;				// type of the instruction.

	// simd-specific fields.
	int has_simd_prefix;			// 0x66, 0xf2, 0xf3 prefixes for simd
	int vex_encoding;				// 0=none, 1=vex, 2=evex
	int simd_size;					// simd operand size (128, 256, 512 bits)
	int simd_type;					// 0=ps, 1=pd, 2=ss, 3=sd, 4=integer
} rda_int_t;

/**
 * @note static table covering most amd64 instructions.
 *
 *	the main reason we have a table like this instead of a
 *	designated initializers for arrays like:
 *
 *	@ref<internal_prefix_table>("prefix table")
 *
 *	where we would compute the first 4 opcodes, is because the
 *	list would grow to a merely ~16GiB and that is not even
 *	worth the size in comparison to something like an array
 *	of ~260 entries with a loop.
 */
static const rda_int_t internal_table[] = {
    // mov/load ops.
    {"mov r/m8, r8",		{0x88}, 1, 0, 0, 1, 0, -1, RDA_INST_TY_DATA},
    {"mov r/m16-64, r16-64",{0x89}, 1, 0, 0, 1, 0, -1, RDA_INST_TY_DATA},
    {"mov r8, r/m8",		{0x8a}, 1, 0, 0, 1, 0, -1, RDA_INST_TY_DATA},
    {"mov r16-64, r/m16-64",{0x8b}, 1, 0, 0, 1, 0, -1, RDA_INST_TY_DATA},
    {"mov r64, imm64",		{0xb8}, 1, 8, 64, 0, 1, -1, RDA_INST_TY_DATA}, // +rd encodings
    {"mov r32, imm32",		{0xb8}, 1, 4, 32, 0, 1, -1, RDA_INST_TY_DATA}, // +rd encodings
    {"mov r/m16-64, imm16",	{0xc7}, 1, -1, 0, 1, 0, 0, RDA_INST_TY_DATA}, // /0
    {"mov r/m8, imm8",		{0xc6}, 1, 1, 8, 1, 0, 0, RDA_INST_TY_DATA}, // /0
    {"lea r16-64, m",		{0x8d}, 1, 0, 0, 1, 0, -1, RDA_INST_TY_DATA},
    {"movzx r16-64, r/m8",  {0x0f,0xb6}, 2, 0, 0, 1, 0, -1, RDA_INST_TY_DATA},
    {"movzx r32-64, r/m16", {0x0f,0xb7}, 2, 0, 0, 1, 0, -1, RDA_INST_TY_DATA},
    {"movsx r16-64, r/m8",  {0x0f,0xbe}, 2, 0, 0, 1, 0, -1, RDA_INST_TY_DATA},
    {"movsx r32-64, r/m16", {0x0f,0xbf}, 2, 0, 0, 1, 0, -1, RDA_INST_TY_DATA},
    {"movsxd r64, r/m32",   {0x63}, 1, 0, 64, 1, 0, -1, RDA_INST_TY_DATA},

    // push/pop ops.
    {"push r64",		{0x50}, 1, 0, 64, 0, 1, -1, RDA_INST_TY_DATA}, // +rd
    {"pop r64",			{0x58}, 1, 0, 64, 0, 1, -1, RDA_INST_TY_DATA}, // +rd
    {"push imm8",		{0x6a}, 1, 1, 0, 0, 0, -1, RDA_INST_TY_DATA},
    {"push imm32",		{0x68}, 1, 4, 0, 0, 0, -1, RDA_INST_TY_DATA},
    {"push r/m16-64",	{0xff}, 1, 0, 0, 1, 0, 6, RDA_INST_TY_DATA}, // /6
    {"pop r/m16-64",	{0x8f}, 1, 0, 0, 1, 0, 0, RDA_INST_TY_DATA}, // /0

    // arithmetic ops.
    {"add r/m8, r8",			{0x00}, 1, 0, 8, 1, 0, -1, RDA_INST_TY_ARITH},
    {"add r/m16-64, r16-64",	{0x01}, 1, 0, 0, 1, 0, -1, RDA_INST_TY_ARITH},
    {"add r8, r/m8",			{0x02}, 1, 0, 8, 1, 0, -1, RDA_INST_TY_ARITH},
    {"add r16-64, r/m16-64",	{0x03}, 1, 0, 0, 1, 0, -1, RDA_INST_TY_ARITH},
    {"add al, imm8",			{0x04}, 1, 1, 8, 0, 0, -1, RDA_INST_TY_ARITH},
    {"add rax, imm32",			{0x05}, 1, 4, 0, 0, 0, -1, RDA_INST_TY_ARITH},
    {"adc r/m8, r8",			{0x10}, 1, 0, 8, 1, 0, -1, RDA_INST_TY_ARITH},
    {"adc r/m16-64, r16-64",	{0x11}, 1, 0, 0, 1, 0, -1, RDA_INST_TY_ARITH},
    {"adc r8, r/m8",			{0x12}, 1, 0, 8, 1, 0, -1, RDA_INST_TY_ARITH},
    {"adc r16-64, r/m16-64",	{0x13}, 1, 0, 0, 1, 0, -1, RDA_INST_TY_ARITH},
    {"sub r/m8, r8",			{0x28}, 1, 0, 8, 1, 0, -1, RDA_INST_TY_ARITH},
    {"sub r/m16-64, r16-64",	{0x29}, 1, 0, 0, 1, 0, -1, RDA_INST_TY_ARITH},
    {"sub r8, r/m8",			{0x2a}, 1, 0, 8, 1, 0, -1, RDA_INST_TY_ARITH},
    {"sub r16-64, r/m16-64",	{0x2b}, 1, 0, 0, 1, 0, -1, RDA_INST_TY_ARITH},
    {"sub al, imm8",			{0x2c}, 1, 1, 8, 0, 0, -1, RDA_INST_TY_ARITH},
    {"sub rax, imm32",			{0x2d}, 1, 4, 0, 0, 0, -1, RDA_INST_TY_ARITH},
    {"sub r/m32, imm32",		{0x81}, 1, 4, 32, 1, 0, 5, RDA_INST_TY_ARITH}, // /5
    {"cmp r/m8, r8",			{0x38}, 1, 0, 8, 1, 0, -1, RDA_INST_TY_ARITH},
    {"cmp r/m16-64, r16-64",	{0x39}, 1, 0, 0, 1, 0, -1, RDA_INST_TY_ARITH},
    {"cmp r8, r/m8",			{0x3a}, 1, 0, 8, 1, 0, -1, RDA_INST_TY_ARITH},
    {"cmp r16-64, r/m16-64",	{0x3b}, 1, 0, 0, 1, 0, -1, RDA_INST_TY_ARITH},
    {"cmp al, imm8",			{0x3c}, 1, 1, 8, 0, 0, -1, RDA_INST_TY_ARITH},
    {"cmp rax, imm32",			{0x3d}, 1, 4, 0, 0, 0, -1, RDA_INST_TY_ARITH},
    {"cmp r/m64, imm32",		{0x81}, 1, 4, 64, 1, 0, 7, RDA_INST_TY_ARITH}, // /7
    {"mul r/m8",				{0xf6}, 1, 0, 8, 1, 0, 4, RDA_INST_TY_ARITH}, // /4
    {"mul r/m16-64",			{0xf7}, 1, 0, 0, 1, 0, 4, RDA_INST_TY_ARITH}, // /4
    {"idiv r/m8",				{0xf6}, 1, 0, 8, 1, 0, 7, RDA_INST_TY_ARITH}, // /7
    {"idiv r/m16-64",			{0xf7}, 1, 0, 0, 1, 0, 7, RDA_INST_TY_ARITH}, // /7
    {"div r/m8",				{0xf6}, 1, 0, 8, 1, 0, 6, RDA_INST_TY_ARITH}, // /6
    {"div r/m16-64",			{0xf7}, 1, 0, 0, 1, 0, 6, RDA_INST_TY_ARITH}, // /6
    {"inc r/m8",				{0xfe}, 1, 0, 8, 1, 0, 0, RDA_INST_TY_ARITH}, // /0
    {"inc r/m16-64",			{0xff}, 1, 0, 0, 1, 0, 0, RDA_INST_TY_ARITH}, // /0
    {"dec r/m8",				{0xfe}, 1, 0, 8, 1, 0, 1, RDA_INST_TY_ARITH}, // /1
    {"dec r/m16-64",			{0xff}, 1, 0, 0, 1, 0, 1, RDA_INST_TY_ARITH}, // /1
    {"imul r16-64, r/m16-64",	{0x0f,0xaf}, 2, 0, 0, 1, 0, -1, RDA_INST_TY_ARITH},

    // logic ops.
    {"and r/m8, r8",			{0x20}, 1, 0, 8, 1, 0, -1, RDA_INST_TY_LOGIC},
    {"and r/m16-64, r16-64",	{0x21}, 1, 0, 0, 1, 0, -1, RDA_INST_TY_LOGIC},
    {"and r8, r/m8",			{0x22}, 1, 0, 8, 1, 0, -1, RDA_INST_TY_LOGIC},
    {"and r16-64, r/m16-64",	{0x23}, 1, 0, 0, 1, 0, -1, RDA_INST_TY_LOGIC},
    {"and al, imm8",			{0x24}, 1, 1, 8, 0, 0, -1, RDA_INST_TY_LOGIC},
    {"and rax, imm32",			{0x25}, 1, 4, 0, 0, 0, -1, RDA_INST_TY_LOGIC},
    {"and r/m16-64, imm32",		{0x81}, 1, 4, 0, 1, 0, 4, RDA_INST_TY_LOGIC}, // /4
    {"or r/m8, r8",				{0x08}, 1, 0, 8, 1, 0, -1, RDA_INST_TY_LOGIC},
    {"or r/m16-64, r16-64",		{0x09}, 1, 0, 0, 1, 0, -1, RDA_INST_TY_LOGIC},
    {"or r8, r/m8",				{0x0a}, 1, 0, 8, 1, 0, -1, RDA_INST_TY_LOGIC},
    {"or r16-64, r/m16-64",		{0x0b}, 1, 0, 0, 1, 0, -1, RDA_INST_TY_LOGIC},
    {"or al, imm8",				{0x0c}, 1, 1, 8, 0, 0, -1, RDA_INST_TY_LOGIC},
    {"or rax, imm32",			{0x0d}, 1, 4, 0, 0, 0, -1, RDA_INST_TY_LOGIC},
    {"or r/m8, imm8",			{0x80}, 1, 1, 8, 1, 0, 1, RDA_INST_TY_LOGIC}, // /1
    {"xor r/m8, r8",			{0x30}, 1, 0, 8, 1, 0, -1, RDA_INST_TY_LOGIC},
    {"xor r/m16-64, r16-64",	{0x31}, 1, 0, 0, 1, 0, -1, RDA_INST_TY_LOGIC},
    {"xor r8, r/m8",			{0x32}, 1, 0, 8, 1, 0, -1, RDA_INST_TY_LOGIC},
    {"xor r16-64, r/m16-64",	{0x33}, 1, 0, 0, 1, 0, -1, RDA_INST_TY_LOGIC},
    {"xor al, imm8",			{0x34}, 1, 1, 8, 0, 0, -1, RDA_INST_TY_LOGIC},
    {"xor rax, imm32",			{0x35}, 1, 4, 0, 0, 0, -1, RDA_INST_TY_LOGIC},
    {"test r/m8, r8",			{0x84}, 1, 0, 8, 1, 0, -1, RDA_INST_TY_LOGIC},
    {"test r/m16-64, r16-64",	{0x85}, 1, 0, 0, 1, 0, -1, RDA_INST_TY_LOGIC},
    {"test al, imm8",			{0xa8}, 1, 1, 8, 0, 0, -1, RDA_INST_TY_LOGIC},
    {"test rax, imm32",			{0xa9}, 1, 4, 0, 0, 0, -1, RDA_INST_TY_LOGIC},
	{"test r/m8, imm8",			{0xf6}, 1, 1, 8, 1, 0, 0, RDA_INST_TY_LOGIC}, // /0
	{"test r/m16-64, imm32",	{0xf7}, 1, 4, 0, 1, 0, 0, RDA_INST_TY_LOGIC}, // /0
    {"not r/m8",				{0xf6}, 1, 0, 8, 1, 0, 2, RDA_INST_TY_LOGIC}, // /2
    {"not r/m16-64",			{0xf7}, 1, 0, 0, 1, 0, 2, RDA_INST_TY_LOGIC}, // /2
    {"neg r/m8",				{0xf6}, 1, 0, 8, 1, 0, 3, RDA_INST_TY_LOGIC}, // /3
    {"neg r/m16-64",			{0xf7}, 1, 0, 0, 1, 0, 3, RDA_INST_TY_LOGIC}, // /3

    // shifts/rotates ops.
    {"shl r/m8, 1",			{0xd0}, 1, 0, 8, 1, 0, 4, RDA_INST_TY_LOGIC}, // /4
    {"shl r/m16-64, 1",		{0xd1}, 1, 0, 0, 1, 0, 4, RDA_INST_TY_LOGIC}, // /4
    {"shl r/m8, cl",		{0xd2}, 1, 0, 8, 1, 0, 4, RDA_INST_TY_LOGIC}, // /4
    {"shl r/m16-64, cl",	{0xd3}, 1, 0, 0, 1, 0, 4, RDA_INST_TY_LOGIC}, // /4
    {"shl r/m8, imm8",		{0xc0}, 1, 1, 8, 1, 0, 4, RDA_INST_TY_LOGIC}, // /4
    {"shl r/m16-64, imm8",	{0xc1}, 1, 1, 0, 1, 0, 4, RDA_INST_TY_LOGIC}, // /4
    {"shr r/m8, 1",			{0xd0}, 1, 0, 8, 1, 0, 5, RDA_INST_TY_LOGIC}, // /5
    {"shr r/m16-64, 1",		{0xd1}, 1, 0, 0, 1, 0, 5, RDA_INST_TY_LOGIC}, // /5
    {"shr r/m8, cl",		{0xd2}, 1, 0, 8, 1, 0, 5, RDA_INST_TY_LOGIC}, // /5
    {"shr r/m16-64, cl",	{0xd3}, 1, 0, 0, 1, 0, 5, RDA_INST_TY_LOGIC}, // /5
    {"shr r/m8, imm8",		{0xc0}, 1, 1, 8, 1, 0, 5, RDA_INST_TY_LOGIC}, // /5
    {"shr r/m16-64, imm8",	{0xc1}, 1, 1, 0, 1, 0, 5, RDA_INST_TY_LOGIC}, // /5
    {"sar r/m8, 1",			{0xd0}, 1, 0, 8, 1, 0, 7, RDA_INST_TY_LOGIC}, // /7
    {"sar r/m16-64, 1",		{0xd1}, 1, 0, 0, 1, 0, 7, RDA_INST_TY_LOGIC}, // /7
    {"sar r/m8, cl",		{0xd2}, 1, 0, 8, 1, 0, 7, RDA_INST_TY_LOGIC}, // /7
    {"sar r/m16-64, cl",	{0xd3}, 1, 0, 0, 1, 0, 7, RDA_INST_TY_LOGIC}, // /7
    {"sar r/m8, imm8",		{0xc0}, 1, 1, 8, 1, 0, 7, RDA_INST_TY_LOGIC}, // /7
    {"sar r/m16-64, imm8",	{0xc1}, 1, 1, 0, 1, 0, 7, RDA_INST_TY_LOGIC}, // /7
    {"rol r/m8, 1",			{0xd0}, 1, 0, 8, 1, 0, 0, RDA_INST_TY_LOGIC}, // /0
    {"rol r/m16-64, 1",		{0xd1}, 1, 0, 0, 1, 0, 0, RDA_INST_TY_LOGIC}, // /0
    {"rol r/m8, cl",		{0xd2}, 1, 0, 8, 1, 0, 0, RDA_INST_TY_LOGIC}, // /0
    {"rol r/m16-64, cl",	{0xd3}, 1, 0, 0, 1, 0, 0, RDA_INST_TY_LOGIC}, // /0
    {"ror r/m8, 1",			{0xd0}, 1, 0, 8, 1, 0, 1, RDA_INST_TY_LOGIC}, // /1
    {"ror r/m16-64, 1",		{0xd1}, 1, 0, 0, 1, 0, 1, RDA_INST_TY_LOGIC}, // /1
    {"ror r/m8, cl",		{0xd2}, 1, 0, 8, 1, 0, 1, RDA_INST_TY_LOGIC}, // /1
    {"ror r/m16-64, cl",	{0xd3}, 1, 0, 0, 1, 0, 1, RDA_INST_TY_LOGIC}, // /1

    // control flow ops.
    {"jmp rel8",	{0xeb}, 1, 1, 0, 0, 0, -1, RDA_INST_TY_CONTROL},
    {"jmp rel32",	{0xe9}, 1, 4, 0, 0, 0, -1, RDA_INST_TY_CONTROL},
	{"jmp ptr16:16",{0xea}, 1, 6, 16, 0, 0, -1, RDA_INST_TY_CONTROL},
	{"jmp ptr16:32",{0xea}, 1, 6, 32, 0, 0, -1, RDA_INST_TY_CONTROL},
    {"jmp r/m64",	{0xff}, 1, 0, 64, 1, 0, 4, RDA_INST_TY_CONTROL}, // /4
    {"call rel32",	{0xe8}, 1, 4, 0, 0, 0, -1, RDA_INST_TY_CONTROL},
    {"call r/m64",	{0xff}, 1, 0, 64, 1, 0, 2, RDA_INST_TY_CONTROL}, // /2
    {"ret",			{0xc3}, 1, 0, 0, 0, 0, -1, RDA_INST_TY_CONTROL},
    {"ret imm16",	{0xc2}, 1, 2, 0, 0, 0, -1, RDA_INST_TY_CONTROL},
    {"retf",		{0xcb}, 1, 0, 0, 0, 0, -1, RDA_INST_TY_CONTROL},
    {"retf imm16",	{0xca}, 1, 2, 0, 0, 0, -1, RDA_INST_TY_CONTROL},

    // conditional jumps (short rel8).
    {"jo rel8",   {0x70}, 1, 1, 0, 0, 0, -1, RDA_INST_TY_CONTROL},
    {"jno rel8",  {0x71}, 1, 1, 0, 0, 0, -1, RDA_INST_TY_CONTROL},
    {"jb rel8",   {0x72}, 1, 1, 0, 0, 0, -1, RDA_INST_TY_CONTROL}, // aka jc, jnae
    {"jnb rel8",  {0x73}, 1, 1, 0, 0, 0, -1, RDA_INST_TY_CONTROL}, // aka jnc, jae
    {"je rel8",   {0x74}, 1, 1, 0, 0, 0, -1, RDA_INST_TY_CONTROL}, // aka jz
    {"jne rel8",  {0x75}, 1, 1, 0, 0, 0, -1, RDA_INST_TY_CONTROL}, // aka jnz
    {"jbe rel8",  {0x76}, 1, 1, 0, 0, 0, -1, RDA_INST_TY_CONTROL}, // aka jna
    {"ja rel8",   {0x77}, 1, 1, 0, 0, 0, -1, RDA_INST_TY_CONTROL}, // aka jnbe
    {"js rel8",   {0x78}, 1, 1, 0, 0, 0, -1, RDA_INST_TY_CONTROL},
    {"jns rel8",  {0x79}, 1, 1, 0, 0, 0, -1, RDA_INST_TY_CONTROL},
    {"jp rel8",   {0x7a}, 1, 1, 0, 0, 0, -1, RDA_INST_TY_CONTROL}, // aka jpe
    {"jnp rel8",  {0x7b}, 1, 1, 0, 0, 0, -1, RDA_INST_TY_CONTROL}, // aka jpo
    {"jl rel8",   {0x7c}, 1, 1, 0, 0, 0, -1, RDA_INST_TY_CONTROL}, // aka jnge
    {"jge rel8",  {0x7d}, 1, 1, 0, 0, 0, -1, RDA_INST_TY_CONTROL}, // aka jnl
    {"jle rel8",  {0x7e}, 1, 1, 0, 0, 0, -1, RDA_INST_TY_CONTROL}, // aka jng
    {"jg rel8",   {0x7f}, 1, 1, 0, 0, 0, -1, RDA_INST_TY_CONTROL}, // aka jnle

    // conditional jumps (near rel32).
    {"jo rel32",   {0x0f,0x80}, 2, 4, 0, 0, 0, -1, RDA_INST_TY_CONTROL},
    {"jno rel32",  {0x0f,0x81}, 2, 4, 0, 0, 0, -1, RDA_INST_TY_CONTROL},
    {"jb rel32",   {0x0f,0x82}, 2, 4, 0, 0, 0, -1, RDA_INST_TY_CONTROL},
    {"jnb rel32",  {0x0f,0x83}, 2, 4, 0, 0, 0, -1, RDA_INST_TY_CONTROL},
    {"je rel32",   {0x0f,0x84}, 2, 4, 0, 0, 0, -1, RDA_INST_TY_CONTROL},
    {"jne rel32",  {0x0f,0x85}, 2, 4, 0, 0, 0, -1, RDA_INST_TY_CONTROL},
    {"jbe rel32",  {0x0f,0x86}, 2, 4, 0, 0, 0, -1, RDA_INST_TY_CONTROL},
    {"ja rel32",   {0x0f,0x87}, 2, 4, 0, 0, 0, -1, RDA_INST_TY_CONTROL},
    {"js rel32",   {0x0f,0x88}, 2, 4, 0, 0, 0, -1, RDA_INST_TY_CONTROL},
    {"jns rel32",  {0x0f,0x89}, 2, 4, 0, 0, 0, -1, RDA_INST_TY_CONTROL},
    {"jp rel32",   {0x0f,0x8a}, 2, 4, 0, 0, 0, -1, RDA_INST_TY_CONTROL},
    {"jnp rel32",  {0x0f,0x8b}, 2, 4, 0, 0, 0, -1, RDA_INST_TY_CONTROL},
    {"jl rel32",   {0x0f,0x8c}, 2, 4, 0, 0, 0, -1, RDA_INST_TY_CONTROL},
    {"jge rel32",  {0x0f,0x8d}, 2, 4, 0, 0, 0, -1, RDA_INST_TY_CONTROL},
    {"jle rel32",  {0x0f,0x8e}, 2, 4, 0, 0, 0, -1, RDA_INST_TY_CONTROL},
    {"jg rel32",   {0x0f,0x8f}, 2, 4, 0, 0, 0, -1, RDA_INST_TY_CONTROL},

    // conditional moves
    {"cmovo r16-64, r/m16-64",	{0x0f,0x40}, 2, 0, 0, 1, 0, -1, RDA_INST_TY_DATA},
    {"cmovno r16-64, r/m16-64", {0x0f,0x41}, 2, 0, 0, 1, 0, -1, RDA_INST_TY_DATA},
    {"cmovb r16-64, r/m16-64",	{0x0f,0x42}, 2, 0, 0, 1, 0, -1, RDA_INST_TY_DATA},
    {"cmovnb r16-64, r/m16-64", {0x0f,0x43}, 2, 0, 0, 1, 0, -1, RDA_INST_TY_DATA},
    {"cmove r16-64, r/m16-64",	{0x0f,0x44}, 2, 0, 0, 1, 0, -1, RDA_INST_TY_DATA},
    {"cmovne r16-64, r/m16-64", {0x0f,0x45}, 2, 0, 0, 1, 0, -1, RDA_INST_TY_DATA},
    {"cmovbe r16-64, r/m16-64", {0x0f,0x46}, 2, 0, 0, 1, 0, -1, RDA_INST_TY_DATA},
    {"cmova r16-64, r/m16-64",	{0x0f,0x47}, 2, 0, 0, 1, 0, -1, RDA_INST_TY_DATA},
    {"cmovs r16-64, r/m16-64",	{0x0f,0x48}, 2, 0, 0, 1, 0, -1, RDA_INST_TY_DATA},
    {"cmovns r16-64, r/m16-64", {0x0f,0x49}, 2, 0, 0, 1, 0, -1, RDA_INST_TY_DATA},
    {"cmovp r16-64, r/m16-64",	{0x0f,0x4a}, 2, 0, 0, 1, 0, -1, RDA_INST_TY_DATA},
    {"cmovnp r16-64, r/m16-64", {0x0f,0x4b}, 2, 0, 0, 1, 0, -1, RDA_INST_TY_DATA},
    {"cmovl r16-64, r/m16-64",	{0x0f,0x4c}, 2, 0, 0, 1, 0, -1, RDA_INST_TY_DATA},
    {"cmovge r16-64, r/m16-64", {0x0f,0x4d}, 2, 0, 0, 1, 0, -1, RDA_INST_TY_DATA},
    {"cmovle r16-64, r/m16-64", {0x0f,0x4e}, 2, 0, 0, 1, 0, -1, RDA_INST_TY_DATA},
    {"cmovg r16-64, r/m16-64",	{0x0f,0x4f}, 2, 0, 0, 1, 0, -1, RDA_INST_TY_DATA},

    // string operations
    {"movs m8, m8",			{0xa4}, 1, 0, 8, 0, 0, -1, RDA_INST_TY_DATA},
    {"movs m16-64, m16-64", {0xa5}, 1, 0, 0, 0, 0, -1, RDA_INST_TY_DATA},
    {"cmps m8, m8",			{0xa6}, 1, 0, 8, 0, 0, -1, RDA_INST_TY_DATA},
    {"cmps m16-64, m16-64", {0xa7}, 1, 0, 0, 0, 0, -1, RDA_INST_TY_DATA},
    {"stos m8",				{0xaa}, 1, 0, 8, 0, 0, -1, RDA_INST_TY_DATA},
    {"stos m16-64",			{0xab}, 1, 0, 0, 0, 0, -1, RDA_INST_TY_DATA},
    {"lods m8",				{0xac}, 1, 0, 8, 0, 0, -1, RDA_INST_TY_DATA},
    {"lods m16-64",			{0xad}, 1, 0, 0, 0, 0, -1, RDA_INST_TY_DATA},
    {"scas m8",				{0xae}, 1, 0, 8, 0, 0, -1, RDA_INST_TY_DATA},
    {"scas m16-64",			{0xaf}, 1, 0, 0, 0, 0, -1, RDA_INST_TY_DATA},

    // stack/flags ops.
    {"pushad",	{0x60}, 1, 0, 32, 0, 0, -1, RDA_INST_TY_FLAG},
    {"popad",	{0x61}, 1, 0, 32, 0, 0, -1, RDA_INST_TY_FLAG},
    {"pushfq",	{0x9c}, 1, 0, 64, 0, 0, -1, RDA_INST_TY_FLAG},
    {"popfq",	{0x9d}, 1, 0, 64, 0, 0, -1, RDA_INST_TY_FLAG},
    {"pushf",	{0x9c}, 1, 0, 16, 0, 0, -1, RDA_INST_TY_FLAG},
    {"popf",	{0x9d}, 1, 0, 16, 0, 0, -1, RDA_INST_TY_FLAG},
    {"enter",	{0xc8}, 1, 3, 0, 0, 0, -1, RDA_INST_TY_FLAG},
    {"leave",	{0xc9}, 1, 0, 0, 0, 0, -1, RDA_INST_TY_FLAG},
    {"clc",		{0xf8}, 1, 0, 0, 0, 0, -1, RDA_INST_TY_FLAG},
    {"stc",		{0xf9}, 1, 0, 0, 0, 0, -1, RDA_INST_TY_FLAG},
    {"cli",		{0xfa}, 1, 0, 0, 0, 0, -1, RDA_INST_TY_FLAG},
    {"sti",		{0xfb}, 1, 0, 0, 0, 0, -1, RDA_INST_TY_FLAG},
    {"cld",		{0xfc}, 1, 0, 0, 0, 0, -1, RDA_INST_TY_FLAG},
    {"std",		{0xfd}, 1, 0, 0, 0, 0, -1, RDA_INST_TY_FLAG},
    {"cmc",		{0xf5}, 1, 0, 0, 0, 0, -1, RDA_INST_TY_FLAG},

    // system/misc ops.
    {"int imm8", {0xcd}, 1, 1, 0, 0, 0, -1, RDA_INST_TY_SYSTEM},
    {"int3",     {0xcc}, 1, 0, 0, 0, 0, -1, RDA_INST_TY_SYSTEM},
    {"int1",     {0xf1}, 1, 0, 0, 0, 0, -1, RDA_INST_TY_SYSTEM}, // icebp (at&t i know)
    {"into",     {0xce}, 1, 0, 0, 0, 0, -1, RDA_INST_TY_SYSTEM},
    {"iret",     {0xcf}, 1, 0, 16, 0, 0, -1, RDA_INST_TY_SYSTEM},
    {"iretd",    {0xcf}, 1, 0, 32, 0, 0, -1, RDA_INST_TY_SYSTEM},
    {"iretq",    {0xcf}, 1, 0, 64, 0, 0, -1, RDA_INST_TY_SYSTEM},
    {"syscall",  {0x0f,0x05}, 2, 0, 0, 0, 0, -1, RDA_INST_TY_SYSTEM},
    {"sysret",   {0x0f,0x07}, 2, 0, 0, 0, 0, -1, RDA_INST_TY_SYSTEM},
    {"sysenter", {0x0f,0x34}, 2, 0, 0, 0, 0, -1, RDA_INST_TY_SYSTEM},
    {"sysexit",  {0x0f,0x35}, 2, 0, 0, 0, 0, -1, RDA_INST_TY_SYSTEM},
    {"hlt",      {0xf4}, 1, 0, 0, 0, 0, -1, RDA_INST_TY_SYSTEM},
    {"nop",      {0x90}, 1, 0, 0, 0, 0, -1, RDA_INST_TY_MISC},
    {"nop r/m16",{0x0f,0x1f}, 2, 0, 16, 1, 0, 0, RDA_INST_TY_MISC}, // /0 multi-byte nop
    {"nop r/m32",{0x0f,0x1f}, 2, 0, 32, 1, 0, 0, RDA_INST_TY_MISC}, // /0 multi-byte nop
	{"pause",    {0xf3,0x90}, 2, 0, 0, 0, 0, -1, RDA_INST_TY_MISC},
    {"ud2",      {0x0f,0x0b}, 2, 0, 0, 0, 0, -1, RDA_INST_TY_MISC},
    {"rdtsc",    {0x0f,0x31}, 2, 0, 0, 0, 0, -1, RDA_INST_TY_MISC},
    {"rdtscp",   {0x0f,0x01,0xf9}, 3, 0, 0, 0, 0, -1, RDA_INST_TY_MISC},

    // loop family.
    {"loop rel8",   {0xe2}, 1, 1, 0, 0, 0, -1, RDA_INST_TY_CONTROL},
    {"loope rel8",  {0xe1}, 1, 1, 0, 0, 0, -1, RDA_INST_TY_CONTROL}, // aka loopz
    {"loopne rel8", {0xe0}, 1, 1, 0, 0, 0, -1, RDA_INST_TY_CONTROL}, // aka loopnz
    {"jecxz rel8",  {0xe3}, 1, 1, 32, 0, 0, -1, RDA_INST_TY_CONTROL},
    {"jrcxz rel8",  {0xe3}, 1, 1, 64, 0, 0, -1, RDA_INST_TY_CONTROL},

    // flag ops.
    {"lahf",		{0x9f}, 1, 0, 0, 0, 0, -1, RDA_INST_TY_FLAG},
    {"sahf",		{0x9e}, 1, 0, 0, 0, 0, -1, RDA_INST_TY_FLAG},
    {"seto r/m8",   {0x0f,0x90}, 2, 0, 8, 1, 0, -1, RDA_INST_TY_FLAG},
    {"setno r/m8",  {0x0f,0x91}, 2, 0, 8, 1, 0, -1, RDA_INST_TY_FLAG},
    {"setb r/m8",   {0x0f,0x92}, 2, 0, 8, 1, 0, -1, RDA_INST_TY_FLAG},
    {"setnb r/m8",  {0x0f,0x93}, 2, 0, 8, 1, 0, -1, RDA_INST_TY_FLAG},
    {"sete r/m8",   {0x0f,0x94}, 2, 0, 8, 1, 0, -1, RDA_INST_TY_FLAG},
    {"setne r/m8",  {0x0f,0x95}, 2, 0, 8, 1, 0, -1, RDA_INST_TY_FLAG},
    {"setbe r/m8",  {0x0f,0x96}, 2, 0, 8, 1, 0, -1, RDA_INST_TY_FLAG},
    {"seta r/m8",   {0x0f,0x97}, 2, 0, 8, 1, 0, -1, RDA_INST_TY_FLAG},
    {"sets r/m8",   {0x0f,0x98}, 2, 0, 8, 1, 0, -1, RDA_INST_TY_FLAG},
    {"setns r/m8",  {0x0f,0x99}, 2, 0, 8, 1, 0, -1, RDA_INST_TY_FLAG},
    {"setp r/m8",   {0x0f,0x9a}, 2, 0, 8, 1, 0, -1, RDA_INST_TY_FLAG},
    {"setnp r/m8",  {0x0f,0x9b}, 2, 0, 8, 1, 0, -1, RDA_INST_TY_FLAG},
    {"setl r/m8",   {0x0f,0x9c}, 2, 0, 8, 1, 0, -1, RDA_INST_TY_FLAG},
    {"setge r/m8",  {0x0f,0x9d}, 2, 0, 8, 1, 0, -1, RDA_INST_TY_FLAG},
    {"setle r/m8",  {0x0f,0x9e}, 2, 0, 8, 1, 0, -1, RDA_INST_TY_FLAG},
    {"setg r/m8",   {0x0f,0x9f}, 2, 0, 8, 1, 0, -1, RDA_INST_TY_FLAG},

    // bit manipulation
    {"bsf r16-64, r/m16-64",	{0x0f,0xbc}, 2, 0, 0, 1, 0, -1, RDA_INST_TY_LOGIC},
    {"bsr r16-64, r/m16-64",	{0x0f,0xbd}, 2, 0, 0, 1, 0, -1, RDA_INST_TY_LOGIC},
    {"bt r/m16, r16",			{0x0f,0xa3}, 2, 0, 16, 1, 0, -1, RDA_INST_TY_LOGIC},
    {"bt r/m32, r32",			{0x0f,0xa3}, 2, 0, 32, 1, 0, -1, RDA_INST_TY_LOGIC},
    {"bt r/m64, r64",			{0x0f,0xa3}, 2, 0, 64, 1, 0, -1, RDA_INST_TY_LOGIC},
    {"bt r/m16, imm8",			{0x0f,0xba}, 2, 1, 16, 1, 0, 4, RDA_INST_TY_LOGIC}, // /4
    {"bt r/m32, imm8",			{0x0f,0xba}, 2, 1, 32, 1, 0, 4, RDA_INST_TY_LOGIC}, // /4
    {"bt r/m64, imm8",			{0x0f,0xba}, 2, 1, 64, 1, 0, 4, RDA_INST_TY_LOGIC}, // /4
    {"bts r/m16, r16",			{0x0f,0xab}, 2, 0, 16, 1, 0, -1, RDA_INST_TY_LOGIC},
    {"bts r/m32, r32",			{0x0f,0xab}, 2, 0, 32, 1, 0, -1, RDA_INST_TY_LOGIC},
    {"bts r/m64, r64",			{0x0f,0xab}, 2, 0, 64, 1, 0, -1, RDA_INST_TY_LOGIC},
    {"btr r/m16, r16",			{0x0f,0xb3}, 2, 0, 16, 1, 0, -1, RDA_INST_TY_LOGIC},
    {"btr r/m32, r32",			{0x0f,0xb3}, 2, 0, 32, 1, 0, -1, RDA_INST_TY_LOGIC},
    {"btr r/m64, r64",			{0x0f,0xb3}, 2, 0, 64, 1, 0, -1, RDA_INST_TY_LOGIC},
    {"btc r/m16, r16",			{0x0f,0xbb}, 2, 0, 16, 1, 0, -1, RDA_INST_TY_LOGIC},
    {"btc r/m32, r32",			{0x0f,0xbb}, 2, 0, 32, 1, 0, -1, RDA_INST_TY_LOGIC},
    {"btc r/m64, r64",			{0x0f,0xbb}, 2, 0, 64, 1, 0, -1, RDA_INST_TY_LOGIC},

    // exchange operations
    {"xchg r/m8, r8",			{0x86}, 1, 0, 8, 1, 0, -1, RDA_INST_TY_DATA}, // intel? also a jk.
    {"xchg r/m16-64, r16-64",	{0x87}, 1, 0, 0, 1, 0, -1, RDA_INST_TY_DATA}, // fpu? im jk.
    {"xchg rax, r64",			{0x90}, 1, 0, 64, 0, 1, -1, RDA_INST_TY_DATA}, // +rd
    {"cmpxchg r/m8, r8",		{0x0f,0xb0}, 2, 0, 8, 1, 0, -1, RDA_INST_TY_DATA},
    {"cmpxchg r/m16-64, r16-64",{0x0f,0xb1}, 2, 0, 0, 1, 0, -1, RDA_INST_TY_DATA},

    // system.
    {"cpuid",	{0x0f,0xa2}, 2, 0, 0, 0, 0, -1, RDA_INST_TY_SYSTEM},
    {"wbinvd",	{0x0f,0x09}, 2, 0, 0, 0, 0, -1, RDA_INST_TY_SYSTEM},
    {"invd",	{0x0f,0x08}, 2, 0, 0, 0, 0, -1, RDA_INST_TY_SYSTEM},
    {"wrmsr",	{0x0f,0x30}, 2, 0, 0, 0, 0, -1, RDA_INST_TY_SYSTEM},
    {"rdmsr",	{0x0f,0x32}, 2, 0, 0, 0, 0, -1, RDA_INST_TY_SYSTEM},

    // cet / endbr (we are going to consider these to be miscellaneous because cet occasionally acts as a nop).
    {"endbr64", {0xf3,0x0f,0x1e,0xfa}, 4, 0, 64, 0, 0, -1, RDA_INST_TY_MISC},
    {"endbr32", {0xf3,0x0f,0x1e,0xfb}, 4, 0, 32, 0, 0, -1, RDA_INST_TY_MISC},

    // load/store operations (these are segment load/store ops, we will also consider these 'data'/'move').
    {"lds r16, m16:16",			{0xc5}, 1, 0, 16, 1, 0, -1, RDA_INST_TY_DATA},
    {"les r16, m16:16",			{0xc4}, 1, 0, 16, 1, 0, -1, RDA_INST_TY_DATA},
    {"lfs r16-64, m16:16-32",	{0x0f,0xb4}, 2, 0, 0, 1, 0, -1, RDA_INST_TY_DATA},
    {"lgs r16-64, m16:16-32",	{0x0f,0xb5}, 2, 0, 0, 1, 0, -1, RDA_INST_TY_DATA},
    {"lss r16-64, m16:16-32",	{0x0f,0xb2}, 2, 0, 0, 1, 0, -1, RDA_INST_TY_DATA},

    // additional common instructions
    {"cwde",	{0x98}, 1, 0, 32, 0, 0, -1, RDA_INST_TY_ARITH},
    {"cdqe",	{0x98}, 1, 0, 64, 0, 0, -1, RDA_INST_TY_ARITH},
    {"cwd",		{0x99}, 1, 0, 16, 0, 0, -1, RDA_INST_TY_ARITH},
    {"cdq",		{0x99}, 1, 0, 32, 0, 0, -1, RDA_INST_TY_ARITH},
    {"cqo",		{0x99}, 1, 0, 64, 0, 0, -1, RDA_INST_TY_ARITH},
    {"xlat",	{0xd7}, 1, 0, 0, 0, 0, -1, RDA_INST_TY_DATA},
    {"wait",	{0x9b}, 1, 0, 0, 0, 0, -1, RDA_INST_TY_MISC}, // technically x87 fpu ? not sure if we will move this or not yet.
    {"fwait",	{0x9b}, 1, 0, 0, 0, 0, -1, RDA_INST_TY_MISC},

	// arithmetic with immediate
	{"add r/m16-64, imm8", {0x83}, 1, 1, 0, 1, 0, 0, RDA_INST_TY_ARITH},  // /0
	{"or r/m16-64, imm8",  {0x83}, 1, 1, 0, 1, 0, 1, RDA_INST_TY_LOGIC},  // /1
	{"adc r/m16-64, imm8", {0x83}, 1, 1, 0, 1, 0, 2, RDA_INST_TY_ARITH},  // /2
	{"sbb r/m16-64, imm8", {0x83}, 1, 1, 0, 1, 0, 3, RDA_INST_TY_ARITH},  // /3
	{"and r/m16-64, imm8", {0x83}, 1, 1, 0, 1, 0, 4, RDA_INST_TY_LOGIC},  // /4
	{"sub r/m16-64, imm8", {0x83}, 1, 1, 0, 1, 0, 5, RDA_INST_TY_ARITH},  // /5
	{"xor r/m16-64, imm8", {0x83}, 1, 1, 0, 1, 0, 6, RDA_INST_TY_LOGIC},  // /6
	{"cmp r/m16-64, imm8", {0x83}, 1, 1, 0, 1, 0, 7, RDA_INST_TY_ARITH},  // /7

	// critical 32 and 8-bit immediate arithmetic
	{"add r/m16-64, imm32", {0x81}, 1, 4, 0, 1, 0, 0, RDA_INST_TY_ARITH}, // /0
	{"or r/m16-64, imm32",  {0x81}, 1, 4, 0, 1, 0, 1, RDA_INST_TY_LOGIC}, // /1
	{"adc r/m16-64, imm32", {0x81}, 1, 4, 0, 1, 0, 2, RDA_INST_TY_ARITH}, // /2
	{"sbb r/m16-64, imm32", {0x81}, 1, 4, 0, 1, 0, 3, RDA_INST_TY_ARITH}, // /3
	{"xor r/m16-64, imm32", {0x81}, 1, 4, 0, 1, 0, 6, RDA_INST_TY_LOGIC}, // /6
	{"add r/m8, imm8",		{0x80}, 1, 1, 8, 1, 0, 0, RDA_INST_TY_ARITH}, // /0
	{"adc r/m8, imm8",		{0x80}, 1, 1, 8, 1, 0, 2, RDA_INST_TY_ARITH}, // /2
	{"sub r/m8, imm8",		{0x80}, 1, 1, 8, 1, 0, 5, RDA_INST_TY_ARITH}, // /5
	{"cmp r/m8, imm8",		{0x80}, 1, 1, 8, 1, 0, 7, RDA_INST_TY_ARITH}, // /7
};
#endif //LRDA_ASMX64_H
