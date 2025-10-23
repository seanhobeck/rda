/**
 *	@author Sean Hobeck
 *	@date 22/10/2025
 *
 *	@file simdx64.h
 */
#ifndef SIMDX64_H
#define SIMDX64_H

/*! @uses rda_int_t, rda_int_ty_t */
#include "asmx64.h"

/**
 * @note static table covering simd amd64 instructions.
 *
 *	this mainly covers sse, sse2, avx, avx2, and avx512 instructions,
 *	yet to include x87 fpu and sse3+. will add more as needed.
 *
 *	a lot of these sse instructions require prefixes, often rex, but
 *	also (0x66, 0xf2, 0xf3) for various instructions.
 *
 *	avx512 instructions use e-vex prefixes which are 4-byte encodings;
 *	they support 512-bit operations of the following formats:
 *
 *		0x62 + [R X B R' 0 0 m m] + [W vvvv 1 p p] + [z L'L b a a a a] + opcodes.
 */
static const rda_int_t internal_simd_table[] = {
	// sse data movement.
    {"movaps xmm1, xmm2/m128",	{0x0f,0x28}, 2, 0, 128, 1, 0, -1, RDA_INST_TY_SSE, 0, 0, 128, 0},
    {"movaps xmm1/m128, xmm2",	{0x0f,0x29}, 2, 0, 128, 1, 0, -1, RDA_INST_TY_SSE, 0, 0, 128, 0},
    {"movups xmm1, xmm2/m128",	{0x0f,0x10}, 2, 0, 128, 1, 0, -1, RDA_INST_TY_SSE, 0, 0, 128, 0},
    {"movups xmm1/m128, xmm2",	{0x0f,0x11}, 2, 0, 128, 1, 0, -1, RDA_INST_TY_SSE, 0, 0, 128, 0},
    {"movss xmm1, xmm2/m32",	{0xf3,0x0f,0x10}, 3, 0, 32, 1, 0, -1, RDA_INST_TY_SSE, 0xf3, 0, 32, 2},
    {"movss xmm1/m32, xmm2",	{0xf3,0x0f,0x11}, 3, 0, 32, 1, 0, -1, RDA_INST_TY_SSE, 0xf3, 0, 32, 2},

    // sse arithmetic.
    {"addps xmm1, xmm2/m128",	{0x0f,0x58}, 2, 0, 128, 1, 0, -1, RDA_INST_TY_SSE, 0, 0, 128, 0},
    {"addss xmm1, xmm2/m32",	{0xf3,0x0f,0x58}, 3, 0, 32, 1, 0, -1, RDA_INST_TY_SSE, 0xf3, 0, 32, 2},
    {"subps xmm1, xmm2/m128",	{0x0f,0x5c}, 2, 0, 128, 1, 0, -1, RDA_INST_TY_SSE, 0, 0, 128, 0},
    {"subss xmm1, xmm2/m32",	{0xf3,0x0f,0x5c}, 3, 0, 32, 1, 0, -1, RDA_INST_TY_SSE, 0xf3, 0, 32, 2},
    {"mulps xmm1, xmm2/m128",	{0x0f,0x59}, 2, 0, 128, 1, 0, -1, RDA_INST_TY_SSE, 0, 0, 128, 0},
    {"mulss xmm1, xmm2/m32",	{0xf3,0x0f,0x59}, 3, 0, 32, 1, 0, -1, RDA_INST_TY_SSE, 0xf3, 0, 32, 2},
    {"divps xmm1, xmm2/m128",	{0x0f,0x5e}, 2, 0, 128, 1, 0, -1, RDA_INST_TY_SSE, 0, 0, 128, 0},
    {"divss xmm1, xmm2/m32",	{0xf3,0x0f,0x5e}, 3, 0, 32, 1, 0, -1, RDA_INST_TY_SSE, 0xf3, 0, 32, 2},

    // sse comparison.
    {"cmpps xmm1, xmm2/m128, imm8",	{0x0f,0xc2}, 2, 1, 128, 1, 0, -1, RDA_INST_TY_SSE, 0, 0, 128, 0},
    {"cmpss xmm1, xmm2/m32, imm8",	{0xf3,0x0f,0xc2}, 3, 1, 32, 1, 0, -1, RDA_INST_TY_SSE, 0xf3, 0, 32, 2},

    // sse logical.
    {"andps xmm1, xmm2/m128",	{0x0f,0x54}, 2, 0, 128, 1, 0, -1, RDA_INST_TY_SSE, 0, 0, 128, 0},
    {"orps xmm1, xmm2/m128",	{0x0f,0x56}, 2, 0, 128, 1, 0, -1, RDA_INST_TY_SSE, 0, 0, 128, 0},
    {"xorps xmm1, xmm2/m128",	{0x0f,0x57}, 2, 0, 128, 1, 0, -1, RDA_INST_TY_SSE, 0, 0, 128, 0},
    {"andnps xmm1, xmm2/m128",	{0x0f,0x55}, 2, 0, 128, 1, 0, -1, RDA_INST_TY_SSE, 0, 0, 128, 0},

    // sse shuffle/unpack.
    {"shufps xmm1, xmm2/m128, imm8",{0x0f,0xc6}, 2, 1, 128, 1, 0, -1, RDA_INST_TY_SSE, 0, 0, 128, 0},
    {"unpckhps xmm1, xmm2/m128",	{0x0f,0x15}, 2, 0, 128, 1, 0, -1, RDA_INST_TY_SSE, 0, 0, 128, 0},
    {"unpcklps xmm1, xmm2/m128",	{0x0f,0x14}, 2, 0, 128, 1, 0, -1, RDA_INST_TY_SSE, 0, 0, 128, 0},

    // sse conversion.
    {"cvtpi2ps xmm, mm/m64",	{0x0f,0x2a}, 2, 0, 128, 1, 0, -1, RDA_INST_TY_SSE, 0, 0, 128, 0},
    {"cvtps2pi mm, xmm/m64",	{0x0f,0x2d}, 2, 0, 64, 1, 0, -1, RDA_INST_TY_SSE, 0, 0, 64, 0},
    {"cvtsi2ss xmm, r/m32",		{0xf3,0x0f,0x2a}, 3, 0, 32, 1, 0, -1, RDA_INST_TY_SSE, 0xf3, 0, 32, 2},
    {"cvtss2si r32, xmm/m32",	{0xf3,0x0f,0x2d}, 3, 0, 32, 1, 0, -1, RDA_INST_TY_SSE, 0xf3, 0, 32, 2},

	// sse2 data movement, double-precision.
    {"movapd xmm1, xmm2/m128",	{0x66,0x0f,0x28}, 3, 0, 128, 1, 0, -1, RDA_INST_TY_SSE2, 0x66, 0, 128, 1},
    {"movapd xmm1/m128, xmm2",	{0x66,0x0f,0x29}, 3, 0, 128, 1, 0, -1, RDA_INST_TY_SSE2, 0x66, 0, 128, 1},
    {"movupd xmm1, xmm2/m128",	{0x66,0x0f,0x10}, 3, 0, 128, 1, 0, -1, RDA_INST_TY_SSE2, 0x66, 0, 128, 1},
    {"movupd xmm1/m128, xmm2",	{0x66,0x0f,0x11}, 3, 0, 128, 1, 0, -1, RDA_INST_TY_SSE2, 0x66, 0, 128, 1},
    {"movsd xmm1, xmm2/m64",	{0xf2,0x0f,0x10}, 3, 0, 64, 1, 0, -1, RDA_INST_TY_SSE2, 0xf2, 0, 64, 3},
    {"movsd xmm1/m64, xmm2",	{0xf2,0x0f,0x11}, 3, 0, 64, 1, 0, -1, RDA_INST_TY_SSE2, 0xf2, 0, 64, 3},

    // sse2 arithmetic, double-precision.
    {"addpd xmm1, xmm2/m128",	{0x66,0x0f,0x58}, 3, 0, 128, 1, 0, -1, RDA_INST_TY_SSE2, 0x66, 0, 128, 1},
    {"addsd xmm1, xmm2/m64",	{0xf2,0x0f,0x58}, 3, 0, 64, 1, 0, -1, RDA_INST_TY_SSE2, 0xf2, 0, 64, 3},
    {"subpd xmm1, xmm2/m128",	{0x66,0x0f,0x5c}, 3, 0, 128, 1, 0, -1, RDA_INST_TY_SSE2, 0x66, 0, 128, 1},
    {"subsd xmm1, xmm2/m64",	{0xf2,0x0f,0x5c}, 3, 0, 64, 1, 0, -1, RDA_INST_TY_SSE2, 0xf2, 0, 64, 3},
    {"mulpd xmm1, xmm2/m128",	{0x66,0x0f,0x59}, 3, 0, 128, 1, 0, -1, RDA_INST_TY_SSE2, 0x66, 0, 128, 1},
    {"mulsd xmm1, xmm2/m64",	{0xf2,0x0f,0x59}, 3, 0, 64, 1, 0, -1, RDA_INST_TY_SSE2, 0xf2, 0, 64, 3},
    {"divpd xmm1, xmm2/m128",	{0x66,0x0f,0x5e}, 3, 0, 128, 1, 0, -1, RDA_INST_TY_SSE2, 0x66, 0, 128, 1},
    {"divsd xmm1, xmm2/m64",	{0xf2,0x0f,0x5e}, 3, 0, 64, 1, 0, -1, RDA_INST_TY_SSE2, 0xf2, 0, 64, 3},

    // sse2 integer simd.
    {"movdqa xmm1, xmm2/m128",	{0x66,0x0f,0x6f}, 3, 0, 128, 1, 0, -1, RDA_INST_TY_SSE2, 0x66, 0, 128, 4},
    {"movdqa xmm1/m128, xmm2",	{0x66,0x0f,0x7f}, 3, 0, 128, 1, 0, -1, RDA_INST_TY_SSE2, 0x66, 0, 128, 4},
    {"movdqu xmm1, xmm2/m128",	{0xf3,0x0f,0x6f}, 3, 0, 128, 1, 0, -1, RDA_INST_TY_SSE2, 0xf3, 0, 128, 4},
    {"movdqu xmm1/m128, xmm2",	{0xf3,0x0f,0x7f}, 3, 0, 128, 1, 0, -1, RDA_INST_TY_SSE2, 0xf3, 0, 128, 4},

    // sse2 packed integer arithmetic.
    {"paddb xmm1, xmm2/m128",	{0x66,0x0f,0xfc}, 3, 0, 128, 1, 0, -1, RDA_INST_TY_SSE2, 0x66, 0, 128, 4},
    {"paddw xmm1, xmm2/m128",	{0x66,0x0f,0xfd}, 3, 0, 128, 1, 0, -1, RDA_INST_TY_SSE2, 0x66, 0, 128, 4},
    {"paddd xmm1, xmm2/m128",	{0x66,0x0f,0xfe}, 3, 0, 128, 1, 0, -1, RDA_INST_TY_SSE2, 0x66, 0, 128, 4},
    {"paddq xmm1, xmm2/m128",	{0x66,0x0f,0xd4}, 3, 0, 128, 1, 0, -1, RDA_INST_TY_SSE2, 0x66, 0, 128, 4},
    {"psubb xmm1, xmm2/m128",	{0x66,0x0f,0xf8}, 3, 0, 128, 1, 0, -1, RDA_INST_TY_SSE2, 0x66, 0, 128, 4},
    {"psubw xmm1, xmm2/m128",	{0x66,0x0f,0xf9}, 3, 0, 128, 1, 0, -1, RDA_INST_TY_SSE2, 0x66, 0, 128, 4},
    {"psubd xmm1, xmm2/m128",	{0x66,0x0f,0xfa}, 3, 0, 128, 1, 0, -1, RDA_INST_TY_SSE2, 0x66, 0, 128, 4},
    {"psubq xmm1, xmm2/m128",	{0x66,0x0f,0xfb}, 3, 0, 128, 1, 0, -1, RDA_INST_TY_SSE2, 0x66, 0, 128, 4},

    // sse2 comparison.
    {"cmppd xmm1, xmm2/m128, imm8", {0x66,0x0f,0xc2}, 3, 1, 128, 1, 0, -1, RDA_INST_TY_SSE2, 0x66, 0, 128, 1},
    {"cmpsd xmm1, xmm2/m64, imm8",  {0xf2,0x0f,0xc2}, 3, 1, 64, 1, 0, -1, RDA_INST_TY_SSE2, 0xf2, 0, 64, 3},

    // sse2 logical.
    {"pand xmm1, xmm2/m128",	{0x66,0x0f,0xdb}, 3, 0, 128, 1, 0, -1, RDA_INST_TY_SSE2, 0x66, 0, 128, 4},
    {"por xmm1, xmm2/m128",		{0x66,0x0f,0xeb}, 3, 0, 128, 1, 0, -1, RDA_INST_TY_SSE2, 0x66, 0, 128, 4},
    {"pxor xmm1, xmm2/m128",    {0x66,0x0f,0xef}, 3, 0, 128, 1, 0, -1, RDA_INST_TY_SSE2, 0x66, 0, 128, 4},
    {"pandn xmm1, xmm2/m128",   {0x66,0x0f,0xdf}, 3, 0, 128, 1, 0, -1, RDA_INST_TY_SSE2, 0x66, 0, 128, 4},

    // sse2 shuffle/unpack.
    {"shufpd xmm1, xmm2/m128, imm8",{0x66,0x0f,0xc6}, 3, 1, 128, 1, 0, -1, RDA_INST_TY_SSE2, 0x66, 0, 128, 1},
    {"unpckhpd xmm1, xmm2/m128",	{0x66,0x0f,0x15}, 3, 0, 128, 1, 0, -1, RDA_INST_TY_SSE2, 0x66, 0, 128, 1},
    {"unpcklpd xmm1, xmm2/m128",	{0x66,0x0f,0x14}, 3, 0, 128, 1, 0, -1, RDA_INST_TY_SSE2, 0x66, 0, 128, 1},

    // sse2 conversion.
    {"cvtsi2sd xmm, r/m32",		{0xf2,0x0f,0x2a}, 3, 0, 64, 1, 0, -1, RDA_INST_TY_SSE2, 0xf2, 0, 64, 3},
    {"cvtsd2si r32, xmm/m64",	{0xf2,0x0f,0x2d}, 3, 0, 64, 1, 0, -1, RDA_INST_TY_SSE2, 0xf2, 0, 64, 3},
    {"cvtps2pd xmm, xmm/m64",	{0x0f,0x5a}, 2, 0, 128, 1, 0, -1, RDA_INST_TY_SSE2, 0, 0, 128, 1},
    {"cvtpd2ps xmm, xmm/m128",	{0x66,0x0f,0x5a}, 3, 0, 128, 1, 0, -1, RDA_INST_TY_SSE2, 0x66, 0, 128, 0},

	// sse3 instructions
    {"addsubps xmm1, xmm2/m128",{0xf2,0x0f,0xd0}, 3, 0, 128, 1, 0, -1, RDA_INST_TY_SSE3, 0xf2, 0, 128, 0},
    {"addsubpd xmm1, xmm2/m128",{0x66,0x0f,0xd0}, 3, 0, 128, 1, 0, -1, RDA_INST_TY_SSE3, 0x66, 0, 128, 1},
    {"haddps xmm1, xmm2/m128",	{0xf2,0x0f,0x7c}, 3, 0, 128, 1, 0, -1, RDA_INST_TY_SSE3, 0xf2, 0, 128, 0},
    {"haddpd xmm1, xmm2/m128",	{0x66,0x0f,0x7c}, 3, 0, 128, 1, 0, -1, RDA_INST_TY_SSE3, 0x66, 0, 128, 1},
    {"hsubps xmm1, xmm2/m128",	{0xf2,0x0f,0x7d}, 3, 0, 128, 1, 0, -1, RDA_INST_TY_SSE3, 0xf2, 0, 128, 0},
    {"hsubpd xmm1, xmm2/m128",	{0x66,0x0f,0x7d}, 3, 0, 128, 1, 0, -1, RDA_INST_TY_SSE3, 0x66, 0, 128, 1},
    {"movshdup xmm1, xmm2/m128",{0xf3,0x0f,0x16}, 3, 0, 128, 1, 0, -1, RDA_INST_TY_SSE3, 0xf3, 0, 128, 0},
    {"movsldup xmm1, xmm2/m128",{0xf3,0x0f,0x12}, 3, 0, 128, 1, 0, -1, RDA_INST_TY_SSE3, 0xf3, 0, 128, 0},
    {"movddup xmm1, xmm2/m64",	{0xf2,0x0f,0x12}, 3, 0, 128, 1, 0, -1, RDA_INST_TY_SSE3, 0xf2, 0, 128, 1},
    {"lddqu xmm1, m128",		{0xf2,0x0f,0xf0}, 3, 0, 128, 1, 0, -1, RDA_INST_TY_SSE3, 0xf2, 0, 128, 4},

    // ssse3 instructions
    {"pshufb xmm1, xmm2/m128",			{0x66,0x0f,0x38,0x00}, 4, 0, 128, 1, 0, -1, RDA_INST_TY_SSSE3, 0x66, 0, 128, 4},
    {"phaddw xmm1, xmm2/m128",			{0x66,0x0f,0x38,0x01}, 4, 0, 128, 1, 0, -1, RDA_INST_TY_SSSE3, 0x66, 0, 128, 4},
    {"phaddd xmm1, xmm2/m128",			{0x66,0x0f,0x38,0x02}, 4, 0, 128, 1, 0, -1, RDA_INST_TY_SSSE3, 0x66, 0, 128, 4},
    {"phaddsw xmm1, xmm2/m128",			{0x66,0x0f,0x38,0x03}, 4, 0, 128, 1, 0, -1, RDA_INST_TY_SSSE3, 0x66, 0, 128, 4},
    {"pmaddubsw xmm1, xmm2/m128",		{0x66,0x0f,0x38,0x04}, 4, 0, 128, 1, 0, -1, RDA_INST_TY_SSSE3, 0x66, 0, 128, 4},
    {"pabsb xmm1, xmm2/m128",			{0x66,0x0f,0x38,0x1c}, 4, 0, 128, 1, 0, -1, RDA_INST_TY_SSSE3, 0x66, 0, 128, 4},
    {"pabsw xmm1, xmm2/m128",			{0x66,0x0f,0x38,0x1d}, 4, 0, 128, 1, 0, -1, RDA_INST_TY_SSSE3, 0x66, 0, 128, 4},
    {"pabsd xmm1, xmm2/m128",			{0x66,0x0f,0x38,0x1e}, 4, 0, 128, 1, 0, -1, RDA_INST_TY_SSSE3, 0x66, 0, 128, 4},
    {"palignr xmm1, xmm2/m128, imm8",	{0x66,0x0f,0x3a,0x0f}, 4, 1, 128, 1, 0, -1, RDA_INST_TY_SSSE3, 0x66, 0, 128, 4},

    // sse4.1 instructions
    {"dpps xmm1, xmm2/m128, imm8",		{0x66,0x0f,0x3a,0x40}, 4, 1, 128, 1, 0, -1, RDA_INST_TY_SSE4_1, 0x66, 0, 128, 0},
    {"dppd xmm1, xmm2/m128, imm8",		{0x66,0x0f,0x3a,0x41}, 4, 1, 128, 1, 0, -1, RDA_INST_TY_SSE4_1, 0x66, 0, 128, 1},
    {"blendps xmm1, xmm2/m128, imm8",	{0x66,0x0f,0x3a,0x0c}, 4, 1, 128, 1, 0, -1, RDA_INST_TY_SSE4_1, 0x66, 0, 128, 0},
    {"blendpd xmm1, xmm2/m128, imm8",	{0x66,0x0f,0x3a,0x0d}, 4, 1, 128, 1, 0, -1, RDA_INST_TY_SSE4_1, 0x66, 0, 128, 1},
    {"pmulld xmm1, xmm2/m128",			{0x66,0x0f,0x38,0x40}, 4, 0, 128, 1, 0, -1, RDA_INST_TY_SSE4_1, 0x66, 0, 128, 4},
    {"pminsd xmm1, xmm2/m128",			{0x66,0x0f,0x38,0x39}, 4, 0, 128, 1, 0, -1, RDA_INST_TY_SSE4_1, 0x66, 0, 128, 4},
    {"pmaxsd xmm1, xmm2/m128",			{0x66,0x0f,0x38,0x3d}, 4, 0, 128, 1, 0, -1, RDA_INST_TY_SSE4_1, 0x66, 0, 128, 4},
    {"roundps xmm1, xmm2/m128, imm8",	{0x66,0x0f,0x3a,0x08}, 4, 1, 128, 1, 0, -1, RDA_INST_TY_SSE4_1, 0x66, 0, 128, 0},
    {"roundpd xmm1, xmm2/m128, imm8",	{0x66,0x0f,0x3a,0x09}, 4, 1, 128, 1, 0, -1, RDA_INST_TY_SSE4_1, 0x66, 0, 128, 1},
    {"ptest xmm1, xmm2/m128",			{0x66,0x0f,0x38,0x17}, 4, 0, 128, 1, 0, -1, RDA_INST_TY_SSE4_1, 0x66, 0, 128, 4},

    // sse4.2 instructions
    {"pcmpgtq xmm1, xmm2/m128",			{0x66,0x0f,0x38,0x37}, 4, 0, 128, 1, 0, -1, RDA_INST_TY_SSE4_2, 0x66, 0, 128, 4},
    {"pcmpestri xmm1, xmm2/m128, imm8", {0x66,0x0f,0x3a,0x61}, 4, 1, 128, 1, 0, -1, RDA_INST_TY_SSE4_2, 0x66, 0, 128, 4},
    {"pcmpestrm xmm1, xmm2/m128, imm8", {0x66,0x0f,0x3a,0x60}, 4, 1, 128, 1, 0, -1, RDA_INST_TY_SSE4_2, 0x66, 0, 128, 4},
    {"crc32 r32, r/m8",					{0xf2,0x0f,0x38,0xf0}, 4, 0, 32, 1, 0, -1, RDA_INST_TY_SSE4_2, 0xf2, 0, 32, 4},
    {"crc32 r32, r/m32",				{0xf2,0x0f,0x38,0xf1}, 4, 0, 32, 1, 0, -1, RDA_INST_TY_SSE4_2, 0xf2, 0, 32, 4},
    {"crc32 r64, r/m64",				{0xf2,0x48,0x0f,0x38,0xf1}, 5, 0, 64, 1, 0, -1, RDA_INST_TY_SSE4_2, 0xf2, 0, 64, 4},
    {"popcnt r16-64, r/m16-64",			{0xf3,0x0f,0xb8}, 3, 0, 0, 1, 0, -1, RDA_INST_TY_SSE4_2, 0xf3, 0, 0, 4},

    // avx2 instructions (vex-encoded 256-bit integer)
    {"vpaddb ymm1, ymm2, ymm3/m256",			{0xc5,0xfd,0xfc}, 3, 0, 256, 1, 0, -1, RDA_INST_TY_AVX2, 0x66, 1, 256, 4},
    {"vpaddw ymm1, ymm2, ymm3/m256",			{0xc5,0xfd,0xfd}, 3, 0, 256, 1, 0, -1, RDA_INST_TY_AVX2, 0x66, 1, 256, 4},
    {"vpaddd ymm1, ymm2, ymm3/m256",			{0xc5,0xfd,0xfe}, 3, 0, 256, 1, 0, -1, RDA_INST_TY_AVX2, 0x66, 1, 256, 4},
    {"vpaddq ymm1, ymm2, ymm3/m256",			{0xc5,0xfd,0xd4}, 3, 0, 256, 1, 0, -1, RDA_INST_TY_AVX2, 0x66, 1, 256, 4},
    {"vpsubb ymm1, ymm2, ymm3/m256",			{0xc5,0xfd,0xf8}, 3, 0, 256, 1, 0, -1, RDA_INST_TY_AVX2, 0x66, 1, 256, 4},
    {"vpsubw ymm1, ymm2, ymm3/m256",			{0xc5,0xfd,0xf9}, 3, 0, 256, 1, 0, -1, RDA_INST_TY_AVX2, 0x66, 1, 256, 4},
    {"vpsubd ymm1, ymm2, ymm3/m256",			{0xc5,0xfd,0xfa}, 3, 0, 256, 1, 0, -1, RDA_INST_TY_AVX2, 0x66, 1, 256, 4},
    {"vpsubq ymm1, ymm2, ymm3/m256",			{0xc5,0xfd,0xfb}, 3, 0, 256, 1, 0, -1, RDA_INST_TY_AVX2, 0x66, 1, 256, 4},
    {"vpmulld ymm1, ymm2, ymm3/m256",			{0xc5,0xfd,0x40}, 3, 0, 256, 1, 0, -1, RDA_INST_TY_AVX2, 0x66, 1, 256, 4},
    {"vpshufb ymm1, ymm2, ymm3/m256",			{0xc5,0xfd,0x00}, 3, 0, 256, 1, 0, -1, RDA_INST_TY_AVX2, 0x66, 1, 256, 4},
    {"vbroadcastss ymm1, m32",					{0xc4,0xe3,0x79,0x18}, 4, 0, 256, 1, 0, -1, RDA_INST_TY_AVX2, 0, 1, 256, 0},
    {"vbroadcastsd ymm1, m64",					{0xc4,0xe3,0x79,0x19}, 4, 0, 256, 1, 0, -1, RDA_INST_TY_AVX2, 0x66, 1, 256, 1},
    {"vperm2i128 ymm1, ymm2, ymm3/m256, imm8",	{0xc4,0xe3,0x79,0x46}, 4, 1, 256, 1, 0, -1, RDA_INST_TY_AVX2, 0x66, 1, 256, 4},
    {"vgatherdps ymm1, [vm32y], ymm2",			{0xc4,0xe2,0x79,0x92}, 4, 0, 256, 1, 0, -1, RDA_INST_TY_AVX2, 0x66, 1, 256, 0},

    // avx512 data movement, evex encoded.
    {"vmovaps zmm1, zmm2/m512",	{0x62,0x81,0x7c,0x28}, 4, 0, 512, 1, 0, -1, RDA_INST_TY_AVX512, 0, 2, 512, 0},
    {"vmovaps zmm1/m512, zmm2",	{0x62,0x81,0x7c,0x29}, 4, 0, 512, 1, 0, -1, RDA_INST_TY_AVX512, 0, 2, 512, 0},
    {"vmovups zmm1, zmm2/m512",	{0x62,0x81,0x7c,0x10}, 4, 0, 512, 1, 0, -1, RDA_INST_TY_AVX512, 0, 2, 512, 0},
    {"vmovups zmm1/m512, zmm2",	{0x62,0x81,0x7c,0x11}, 4, 0, 512, 1, 0, -1, RDA_INST_TY_AVX512, 0, 2, 512, 0},
    {"vmovapd zmm1, zmm2/m512",	{0x62,0x81,0x7d,0x28}, 4, 0, 512, 1, 0, -1, RDA_INST_TY_AVX512, 0x66, 2, 512, 1},
    {"vmovapd zmm1/m512, zmm2",	{0x62,0x81,0x7d,0x29}, 4, 0, 512, 1, 0, -1, RDA_INST_TY_AVX512, 0x66, 2, 512, 1},
    {"vmovdqa32 zmm1, zmm2/m512", {0x62,0x81,0x7d,0x6f}, 4, 0, 512, 1, 0, -1, RDA_INST_TY_AVX512, 0x66, 2, 512, 4},
    {"vmovdqa32 zmm1/m512, zmm2", {0x62,0x81,0x7d,0x7f}, 4, 0, 512, 1, 0, -1, RDA_INST_TY_AVX512, 0x66, 2, 512, 4},
    {"vmovdqu32 zmm1, zmm2/m512", {0x62,0x81,0x7e,0x6f}, 4, 0, 512, 1, 0, -1, RDA_INST_TY_AVX512, 0xf3, 2, 512, 4},
    {"vmovdqu32 zmm1/m512, zmm2", {0x62,0x81,0x7e,0x7f}, 4, 0, 512, 1, 0, -1, RDA_INST_TY_AVX512, 0xf3, 2, 512, 4},

    // avx512 arithmetic, evex encoded.
    {"vaddps zmm1, zmm2, zmm3/m512",	{0x62,0x81,0x7c,0x58}, 4, 0, 512, 1, 0, -1, RDA_INST_TY_AVX512, 0, 2, 512, 0},
    {"vaddpd zmm1, zmm2, zmm3/m512",	{0x62,0x81,0x7d,0x58}, 4, 0, 512, 1, 0, -1, RDA_INST_TY_AVX512, 0x66, 2, 512, 1},
    {"vsubps zmm1, zmm2, zmm3/m512",	{0x62,0x81,0x7c,0x5c}, 4, 0, 512, 1, 0, -1, RDA_INST_TY_AVX512, 0, 2, 512, 0},
    {"vsubpd zmm1, zmm2, zmm3/m512",	{0x62,0x81,0x7d,0x5c}, 4, 0, 512, 1, 0, -1, RDA_INST_TY_AVX512, 0x66, 2, 512, 1},
    {"vmulps zmm1, zmm2, zmm3/m512",	{0x62,0x81,0x7c,0x59}, 4, 0, 512, 1, 0, -1, RDA_INST_TY_AVX512, 0, 2, 512, 0},
    {"vmulpd zmm1, zmm2, zmm3/m512",	{0x62,0x81,0x7d,0x59}, 4, 0, 512, 1, 0, -1, RDA_INST_TY_AVX512, 0x66, 2, 512, 1},
    {"vdivps zmm1, zmm2, zmm3/m512",	{0x62,0x81,0x7c,0x5e}, 4, 0, 512, 1, 0, -1, RDA_INST_TY_AVX512, 0, 2, 512, 0},
    {"vdivpd zmm1, zmm2, zmm3/m512",	{0x62,0x81,0x7d,0x5e}, 4, 0, 512, 1, 0, -1, RDA_INST_TY_AVX512, 0x66, 2, 512, 1},

    // avx512 integer arithmetic, evex encoded.
    {"vpaddd zmm1, zmm2, zmm3/m512", {0x62,0x81,0x7d,0xfe}, 4, 0, 512, 1, 0, -1, RDA_INST_TY_AVX512, 0x66, 2, 512, 4},
    {"vpaddq zmm1, zmm2, zmm3/m512", {0x62,0x81,0xfd,0xd4}, 4, 0, 512, 1, 0, -1, RDA_INST_TY_AVX512, 0x66, 2, 512, 4},
    {"vpsubd zmm1, zmm2, zmm3/m512", {0x62,0x81,0x7d,0xfa}, 4, 0, 512, 1, 0, -1, RDA_INST_TY_AVX512, 0x66, 2, 512, 4},
    {"vpsubq zmm1, zmm2, zmm3/m512", {0x62,0x81,0xfd,0xfb}, 4, 0, 512, 1, 0, -1, RDA_INST_TY_AVX512, 0x66, 2, 512, 4},
    {"vpmulld zmm1, zmm2, zmm3/m512", {0x62,0x82,0x7d,0x40}, 4, 0, 512, 1, 0, -1, RDA_INST_TY_AVX512, 0x66, 2, 512, 4}, // 0f 38 map

    // avx512 comparison, evex encoded.
    {"vcmpps k1, zmm2, zmm3/m512, imm8",	{0x62,0x81,0x7c,0xc2}, 4, 1, 512, 1, 0, -1, RDA_INST_TY_AVX512, 0, 2, 512, 0},
    {"vcmppd k1, zmm2, zmm3/m512, imm8",	{0x62,0x81,0x7d,0xc2}, 4, 1, 512, 1, 0, -1, RDA_INST_TY_AVX512, 0x66, 2, 512, 1},

    // avx512 logical, evex encoded.
    {"vpandd zmm1, zmm2, zmm3/m512",	{0x62,0x81,0x7d,0xdb}, 4, 0, 512, 1, 0, -1, RDA_INST_TY_AVX512, 0x66, 2, 512, 4},
    {"vpandq zmm1, zmm2, zmm3/m512",	{0x62,0x81,0xfd,0xdb}, 4, 0, 512, 1, 0, -1, RDA_INST_TY_AVX512, 0x66, 2, 512, 4},
    {"vpord zmm1, zmm2, zmm3/m512",		{0x62,0x81,0x7d,0xeb}, 4, 0, 512, 1, 0, -1, RDA_INST_TY_AVX512, 0x66, 2, 512, 4},
    {"vporq zmm1, zmm2, zmm3/m512",		{0x62,0x81,0xfd,0xeb}, 4, 0, 512, 1, 0, -1, RDA_INST_TY_AVX512, 0x66, 2, 512, 4},
    {"vpxord zmm1, zmm2, zmm3/m512",	{0x62,0x81,0x7d,0xef}, 4, 0, 512, 1, 0, -1, RDA_INST_TY_AVX512, 0x66, 2, 512, 4},
    {"vpxorq zmm1, zmm2, zmm3/m512",	{0x62,0x81,0xfd,0xef}, 4, 0, 512, 1, 0, -1, RDA_INST_TY_AVX512, 0x66, 2, 512, 4},

    // avx512 mask operations, vex-encoded.
    {"kmovb k1, k2/m8",		{0xc5,0xf9,0x90}, 3, 0, 8, 1, 0, -1, RDA_INST_TY_AVX512, 0x66, 1, 8, 4},
    {"kmovw k1, k2/m16",	{0xc5,0xf9,0x90}, 3, 0, 16, 1, 0, -1, RDA_INST_TY_AVX512, 0x66, 1, 16, 4},
    {"kmovd k1, k2/m32",	{0xc5,0x79,0x90}, 3, 0, 32, 1, 0, -1, RDA_INST_TY_AVX512, 0x66, 1, 32, 4}, // L=1
    {"kmovq k1, k2/m64",	{0xc5,0x39,0x90}, 3, 0, 64, 1, 0, -1, RDA_INST_TY_AVX512, 0x66, 1, 64, 4}, // L=2
    {"kandb k1, k2, k3",	{0xc5,0xfd,0x41}, 3, 0, 8, 1, 0, -1, RDA_INST_TY_AVX512, 0x66, 1, 8, 4},
    {"kandd k1, k2, k3",	{0xc5,0x7d,0x41}, 3, 0, 32, 1, 0, -1, RDA_INST_TY_AVX512, 0x66, 1, 32, 4},

    // avx512 shuffle/unpack, evex encoded.
    {"vshufps zmm1, zmm2, zmm3/m512, imm8",	{0x62,0x81,0x7c,0xc6}, 4, 1, 512, 1, 0, -1, RDA_INST_TY_AVX512, 0, 2, 512, 0},
    {"vshufpd zmm1, zmm2, zmm3/m512, imm8",	{0x62,0x81,0x7d,0xc6}, 4, 1, 512, 1, 0, -1, RDA_INST_TY_AVX512, 0x66, 2, 512, 1},
    {"vunpckhps zmm1, zmm2, zmm3/m512",		{0x62,0x81,0x7c,0x15}, 4, 0, 512, 1, 0, -1, RDA_INST_TY_AVX512, 0, 2, 512, 0},
    {"vunpcklps zmm1, zmm2, zmm3/m512",		{0x62,0x81,0x7c,0x14}, 4, 0, 512, 1, 0, -1, RDA_INST_TY_AVX512, 0, 2, 512, 0},

    // avx512 conversion, evex encoded.
    {"vcvtps2pd zmm1, ymm2/m256",	{0x62,0x81,0x7c,0x5a}, 4, 0, 512, 1, 0, -1, RDA_INST_TY_AVX512, 0, 2, 512, 1},
    {"vcvtpd2ps ymm1, zmm2/m512",	{0x62,0x81,0x7d,0x5a}, 4, 0, 256, 1, 0, -1, RDA_INST_TY_AVX512, 0x66, 2, 256, 0},
    {"vcvtsi2ss xmm1, xmm2, r/m32", {0x62,0x81,0x7e,0x2a}, 4, 0, 32, 1, 0, -1, RDA_INST_TY_AVX512, 0xf3, 2, 32, 2},
    {"vcvtsi2sd xmm1, xmm2, r/m32", {0x62,0x81,0x7f,0x2a}, 4, 0, 64, 1, 0, -1, RDA_INST_TY_AVX512, 0xf2, 2, 64, 3},
    {"vcvtss2si r32, xmm1/m32",		{0x62,0x81,0x7e,0x2d}, 4, 0, 32, 1, 0, -1, RDA_INST_TY_AVX512, 0xf3, 2, 32, 2},
    {"vcvtsd2si r32, xmm1/m64",		{0x62,0x81,0x7f,0x2d}, 4, 0, 64, 1, 0, -1, RDA_INST_TY_AVX512, 0xf2, 2, 64, 3},

	// avx vex-encoded 128-bit data movement
	{"vmovaps xmm1, xmm2/m128",     {0xc5,0xf8,0x28}, 3, 0, 128, 1, 0, -1, RDA_INST_TY_AVX, 0, 1, 128, 0},
	{"vmovaps xmm1/m128, xmm2",     {0xc5,0xf9,0x29}, 3, 0, 128, 1, 0, -1, RDA_INST_TY_AVX, 0, 1, 128, 0},
	{"vmovups xmm1, xmm2/m128",     {0xc5,0xf8,0x10}, 3, 0, 128, 1, 0, -1, RDA_INST_TY_AVX, 0, 1, 128, 0},
	{"vmovups xmm1/m128, xmm2",     {0xc5,0xf9,0x11}, 3, 0, 128, 1, 0, -1, RDA_INST_TY_AVX, 0, 1, 128, 0},

	// avx vex-encoded 256-bit data movement
	{"vmovaps ymm1, ymm2/m256",     {0xc5,0xfc,0x28}, 3, 0, 256, 1, 0, -1, RDA_INST_TY_AVX, 0, 1, 256, 0},
	{"vmovaps ymm1/m256, ymm2",     {0xc5,0xfd,0x29}, 3, 0, 256, 1, 0, -1, RDA_INST_TY_AVX, 0, 1, 256, 0},

	// avx vex-encoded arithmetic
	{"vaddps xmm1, xmm2, xmm3/m128", {0xc5,0xf0,0x58}, 3, 0, 128, 1, 0, -1, RDA_INST_TY_AVX, 0, 1, 128, 0},
	{"vaddps ymm1, ymm2, ymm3/m256", {0xc5,0xf4,0x58}, 3, 0, 256, 1, 0, -1, RDA_INST_TY_AVX, 0, 1, 256, 0},
	{"vaddpd xmm1, xmm2, xmm3/m128", {0xc5,0xf1,0x58}, 3, 0, 128, 1, 0, -1, RDA_INST_TY_AVX, 0x66, 1, 128, 1},
	{"vaddpd ymm1, ymm2, ymm3/m256", {0xc5,0xf5,0x58}, 3, 0, 256, 1, 0, -1, RDA_INST_TY_AVX, 0x66, 1, 256, 1},

	// avx vex-encoded integer simd
	{"vmovdqu xmm1, xmm2/m128",			{0xc5,0xf9,0x6f}, 3, 0, 128, 1, 0, -1, RDA_INST_TY_AVX, 0xf3, 1, 128, 4},
	{"vmovdqu xmm1/m128, xmm2",			{0xc5,0xf9,0x7f}, 3, 0, 128, 1, 0, -1, RDA_INST_TY_AVX, 0xf3, 1, 128, 4},
	{"vmovdqa xmm1, xmm2/m128",			{0xc5,0xf9,0x6f}, 3, 0, 128, 1, 0, -1, RDA_INST_TY_AVX, 0x66, 1, 128, 4},
	{"vmovdqa xmm1/m128, xmm2",			{0xc5,0xf9,0x7f}, 3, 0, 128, 1, 0, -1, RDA_INST_TY_AVX, 0x66, 1, 128, 4},
	{"vpaddd xmm1, xmm2, xmm3/m128",	{0xc5,0xf9,0xfe}, 3, 0, 128, 1, 0, -1, RDA_INST_TY_AVX, 0x66, 1, 128, 4},
	{"vpsubd xmm1, xmm2, xmm3/m128",	{0xc5,0xf9,0xfa}, 3, 0, 128, 1, 0, -1, RDA_INST_TY_AVX, 0x66, 1, 128, 4},
	{"vpmulld xmm1, xmm2, xmm3/m128",	{0xc5,0xf9,0x40}, 3, 0, 128, 1, 0, -1, RDA_INST_TY_AVX, 0x66, 1, 128, 4},

	// additional vex-encoded instructions that compilers commonly generate
	{"vmovups xmm1, xmm2/m128", {0xc5,0xf8,0x10}, 3, 0, 128, 1, 0, -1, RDA_INST_TY_AVX, 0, 1, 128, 0},
	{"vmovups xmm1/m128, xmm2", {0xc5,0xf8,0x11}, 3, 0, 128, 1, 0, -1, RDA_INST_TY_AVX, 0, 1, 128, 0},

	// avx vex-encoded scalar operations
	{"vmovss xmm1, xmm2/m32",       {0xc5,0xfa,0x10}, 3, 0, 32, 1, 0, -1, RDA_INST_TY_AVX, 0, 1, 32, 2},
	{"vmovss xmm1/m32, xmm2",       {0xc5,0xfa,0x11}, 3, 0, 32, 1, 0, -1, RDA_INST_TY_AVX, 0, 1, 32, 2},
	{"vmovsd xmm1, xmm2/m64",       {0xc5,0xfb,0x10}, 3, 0, 64, 1, 0, -1, RDA_INST_TY_AVX, 0, 1, 64, 3},
	{"vmovsd xmm1/m64, xmm2",       {0xc5,0xfb,0x11}, 3, 0, 64, 1, 0, -1, RDA_INST_TY_AVX, 0, 1, 64, 3},
	{"vmulss xmm1, xmm2, xmm3/m32", {0xc5,0xea,0x59}, 3, 0, 32, 1, 0, -1, RDA_INST_TY_AVX, 0, 1, 32, 2},
	{"vmulsd xmm1, xmm2, xmm3/m64", {0xc5,0xeb,0x59}, 3, 0, 64, 1, 0, -1, RDA_INST_TY_AVX, 0, 1, 64, 3},
	{"vaddss xmm1, xmm2, xmm3/m32", {0xc5,0xea,0x58}, 3, 0, 32, 1, 0, -1, RDA_INST_TY_AVX, 0, 1, 32, 2},
	{"vaddsd xmm1, xmm2, xmm3/m64", {0xc5,0xeb,0x58}, 3, 0, 64, 1, 0, -1, RDA_INST_TY_AVX, 0, 1, 64, 3},
	{"vdivsd xmm1, xmm2, xmm3/m64", {0xc5,0xeb,0x5e}, 3, 0, 64, 1, 0, -1, RDA_INST_TY_AVX, 0, 1, 64, 3},
	{"vcvtss2sd xmm1, xmm2, xmm3/m32", {0xc5,0xea,0x5a}, 3, 0, 32, 1, 0, -1, RDA_INST_TY_AVX, 0, 1, 32, 2},
};
#define RDA_INT_SIMD_TABLE_SIZE sizeof(internal_simd_table) / sizeof(amd64_int_t)
#endif //SIMDX64_H