/**
 *
 *	@author Sean Hobeck
 *	@date 15/09/2025
 *
 */
#ifndef LRDA_AMD64_H
#define LRDA_AMD64_H

/// @note a amd64/x86_64 representation for an instruction.
typedef struct {
	const char* mnemonic; 		// mnemonic for the instruction.
	unsigned char opcode[3];	// 3 starting opcodes.
	int olen, ilen, size, modrm;// opcode/immediate length, opcode size, and the modr/m.
} i_amd64_t;

/// @note static table covering most amd64 instructions.
static const i_amd64_t table_amd64[] ={
    // mov/load ops.
    {"mov r/m8, r8",   {0x88}, 1, 1, 0, 8},
    {"mov r/m16-64, r16-64", {0x89}, 1, 1, 0, 0},
    {"mov r8, r/m8",   {0x8a}, 1, 1, 0, 8},
    {"mov r16-64, r/m16-64", {0x8b}, 1, 1, 0, 0},
    {"mov rax..rdi, imm", {0xb8}, 1, 0, -1, 0}, // +rd encodings
    {"mov r/m16-64, imm", {0xc7}, 1, 1, -1, 0}, // imm16/32
    {"mov r/m8, imm8", {0xc6}, 1, 1, 1, 8},
    {"lea r16-64, m",  {0x8d}, 1, 1, 0, 0},
    {"movzx r16-64, r/m8", {0x0f,0xb6}, 2, 1, 0, 0},
    {"movzx r32-64, r/m16", {0x0f,0xb7}, 2, 1, 0, 0},
    {"movsx r16-64, r/m8", {0x0f,0xbe}, 2, 1, 0, 0},
    {"movsx r32-64, r/m16", {0x0f,0xbf}, 2, 1, 0, 0},
    {"movsxd r64, r/m32", {0x63}, 1, 1, 0, 64},

    // push/pop ops.
    {"push r64", {0x50}, 1, 0, 0, 64}, // +rd
    {"pop r64",  {0x58}, 1, 0, 0, 64}, // +rd
    {"push imm8",  {0x6a}, 1, 0, 1, 0},
    {"push imm32", {0x68}, 1, 0, 4, 0},
    {"push r/m16-64", {0xff}, 1, 1, 0, 0},
    {"pop r/m16-64",  {0x8f}, 1, 1, 0, 0},

    // arithmetic ops.
    {"add r/m8, r8", {0x00}, 1, 1, 0, 8},
    {"adc r/m16-64, r16-64", {0x11}, 1, 1, 0, 0},
    {"sub r/m32, imm32", {0x81}, 1, 1, 4, 32},
    {"cmp r/m64, imm32", {0x81}, 1, 1, 4, 64}, // /7
    {"imul r16-64, r/m16-64", {0x0f,0xaf}, 2, 1, 0, 0},
    {"idiv r/m8", {0xf6}, 1, 1, 0, 8}, // /7
    {"idiv r/m16-64", {0xf7}, 1, 1, 0, 0}, // /7
    {"inc r/m8", {0xfe}, 1, 1, 0, 8},
    {"dec r/m64", {0xff}, 1, 1, 0, 64},

    // logic ops.
    {"and r/m16-64, imm32", {0x81}, 1, 1, 4, 0}, // /4
    {"or r/m8, imm8", {0x80}, 1, 1, 1, 8},
    {"xor r/m32, r32", {0x31}, 1, 1, 0, 32},
    {"test r/m64, r64", {0x85}, 1, 1, 0, 64},

    // shifts/rotates ops.
    {"shl r/m8, 1", {0xd0}, 1, 1, 0, 8},  // /4
    {"shr r/m32, cl", {0xd3}, 1, 1, 0, 32}, // /5
    {"sar r/m64, imm8", {0xc1}, 1, 1, 1, 64}, // /7

    // control flow ops.
    {"jmp rel8",  {0xeb}, 1, 0, 1, 0},
    {"jmp rel32", {0xe9}, 1, 0, 4, 0},
    {"jmp r/m64", {0xff}, 1, 1, 0, 64}, // /4
    {"call rel32", {0xe8}, 1, 0, 4, 0},
    {"call r/m64", {0xff}, 1, 1, 0, 64}, // /2
    {"ret",       {0xc3}, 1, 0, 0, 0},
    {"ret imm16", {0xc2}, 1, 0, 2, 0},

    {"je rel8", {0x74}, 1, 0, 1, 0},
    {"jne rel8", {0x75}, 1, 0, 1, 0},
    {"jg rel8", {0x7f}, 1, 0, 1, 0},
    {"jl rel8", {0x7c}, 1, 0, 1, 0},
    {"ja rel8", {0x77}, 1, 0, 1, 0},
    {"jb rel8", {0x72}, 1, 0, 1, 0},

	// conditional jumps (short rel8).
	{"jo rel8",   {0x70}, 1, 0, 1, 0},
	{"jno rel8",  {0x71}, 1, 0, 1, 0},
	{"jb rel8",   {0x72}, 1, 0, 1, 0}, // aka jc, jnae
	{"jnb rel8",  {0x73}, 1, 0, 1, 0}, // aka jnc, jae
	{"je rel8",   {0x74}, 1, 0, 1, 0}, // aka jz
	{"jne rel8",  {0x75}, 1, 0, 1, 0}, // aka jnz
	{"jbe rel8",  {0x76}, 1, 0, 1, 0}, // aka jna
	{"ja rel8",   {0x77}, 1, 0, 1, 0}, // aka jnbe
	{"js rel8",   {0x78}, 1, 0, 1, 0},
	{"jns rel8",  {0x79}, 1, 0, 1, 0},
	{"jp rel8",   {0x7a}, 1, 0, 1, 0}, // aka jpe
	{"jnp rel8",  {0x7b}, 1, 0, 1, 0}, // aka jpo
	{"jl rel8",   {0x7c}, 1, 0, 1, 0}, // aka jnge
	{"jge rel8",  {0x7d}, 1, 0, 1, 0}, // aka jnl
	{"jle rel8",  {0x7e}, 1, 0, 1, 0}, // aka jng
	{"jg rel8",   {0x7f}, 1, 0, 1, 0}, // aka jnle

	// conditional jumps (near rel32)
	{"jo rel32",   {0x0f,0x80}, 2, 0, 4, 0},
	{"jno rel32",  {0x0f,0x81}, 2, 0, 4, 0},
	{"jb rel32",   {0x0f,0x82}, 2, 0, 4, 0},
	{"jnb rel32",  {0x0f,0x83}, 2, 0, 4, 0},
	{"je rel32",   {0x0f,0x84}, 2, 0, 4, 0},
	{"jne rel32",  {0x0f,0x85}, 2, 0, 4, 0},
	{"jbe rel32",  {0x0f,0x86}, 2, 0, 4, 0},
	{"ja rel32",   {0x0f,0x87}, 2, 0, 4, 0},
	{"js rel32",   {0x0f,0x88}, 2, 0, 4, 0},
	{"jns rel32",  {0x0f,0x89}, 2, 0, 4, 0},
	{"jp rel32",   {0x0f,0x8a}, 2, 0, 4, 0},
	{"jnp rel32",  {0x0f,0x8b}, 2, 0, 4, 0},
	{"jl rel32",   {0x0f,0x8c}, 2, 0, 4, 0},
	{"jge rel32",  {0x0f,0x8d}, 2, 0, 4, 0},
	{"jle rel32",  {0x0f,0x8e}, 2, 0, 4, 0},
	{"jg rel32",   {0x0f,0x8f}, 2, 0, 4, 0},

    // stack/flags ops.
    {"enter", {0xc8}, 1, 0, 3, 0},
    {"leave", {0xc9}, 1, 0, 0, 0},
    {"pushf", {0x9c}, 1, 0, 0, 0},
    {"popf",  {0x9d}, 1, 0, 0, 0},
    {"clc",   {0xf8}, 1, 0, 0, 0},
    {"stc",   {0xf9}, 1, 0, 0, 0},
    {"cmc",   {0xf5}, 1, 0, 0, 0},

    // system/misc ops.
    {"int imm8", {0xcd}, 1, 0, 1, 0},
    {"syscall", {0x0f,0x05}, 2, 0, 0, 0},
    {"sysret",  {0x0f,0x07}, 2, 0, 0, 0},
    {"hlt",     {0xf4}, 1, 0, 0, 0},
    {"nop",     {0x90}, 1, 0, 0, 0},
    {"ud2",     {0x0f,0x0b}, 2, 0, 0, 0},
};
#endif //LRDA_AMD64_H
