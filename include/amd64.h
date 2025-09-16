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
	unsigned char opcode[4];	// 4 starting opcodes.
	int olen, ilen, size, modrm;// opcode/immediate length, opcode size, and the modr/m.
	int plus_reg, modrm_reg;
} i_amd64_t;

/// @note static table covering most amd64 instructions.
static const i_amd64_t table_amd64[] = {
    // mov/load ops.
    {"mov r/m8, r8",   {0x88}, 1, 1, 0, 8, -1, -1},
    {"mov r/m16-64, r16-64", {0x89}, 1, 1, 0, 0, -1, -1},
    {"mov r8, r/m8",   {0x8A}, 1, 1, 0, 8, -1, -1},
    {"mov r16-64, r/m16-64", {0x8B}, 1, 1, 0, 0, -1, -1},
    {"mov r64, imm",   {0xB8}, 1, 0, -1, 64, 1, -1}, // +rd encodings
    {"mov r/m16-64, imm", {0xC7}, 1, 1, -1, 0, -1, 0}, // /0
    {"mov r/m8, imm8", {0xC6}, 1, 1, 1, 8, -1, 0},    // /0
    {"lea r16-64, m",  {0x8D}, 1, 1, 0, 0, -1, -1},
    {"movzx r16-64, r/m8",  {0x0F,0xB6}, 2, 1, 0, 0, -1, -1},
    {"movzx r32-64, r/m16", {0x0F,0xB7}, 2, 1, 0, 0, -1, -1},
    {"movsx r16-64, r/m8",  {0x0F,0xBE}, 2, 1, 0, 0, -1, -1},
    {"movsx r32-64, r/m16", {0x0F,0xBF}, 2, 1, 0, 0, -1, -1},
    {"movsxd r64, r/m32",   {0x63}, 1, 1, 0, 64, -1, -1},

    // push/pop ops.
    {"push r64", {0x50}, 1, 0, 0, 64, 1, -1}, // +rd
    {"pop r64",  {0x58}, 1, 0, 0, 64, 1, -1}, // +rd
    {"push imm8",  {0x6A}, 1, 0, 1, 0, -1, -1},
    {"push imm32", {0x68}, 1, 0, 4, 0, -1, -1},
    {"push r/m16-64", {0xFF}, 1, 1, 0, 0, -1, 6}, // /6
    {"pop r/m16-64",  {0x8F}, 1, 1, 0, 0, -1, 0}, // /0

    // arithmetic ops.
    {"add r/m8, r8", {0x00}, 1, 1, 0, 8, -1, -1},
    {"adc r/m16-64, r16-64", {0x11}, 1, 1, 0, 0, -1, -1},
    {"sub r/m32, imm32", {0x81}, 1, 1, 4, 32, -1, 5}, // /5
    {"cmp r/m64, imm32", {0x81}, 1, 1, 4, 64, -1, 7}, // /7
    {"imul r16-64, r/m16-64", {0x0F,0xAF}, 2, 1, 0, 0, -1, -1},
    {"idiv r/m8",  {0xF6}, 1, 1, 0, 8, -1, 7},  // /7
    {"idiv r/m16-64", {0xF7}, 1, 1, 0, 0, -1, 7}, // /7
    {"inc r/m8", {0xFE}, 1, 1, 0, 8, -1, 0},    // /0
    {"dec r/m64", {0xFF}, 1, 1, 0, 64, -1, 1}, // /1

    // logic ops.
    {"and r/m16-64, imm32", {0x81}, 1, 1, 4, 0, -1, 4}, // /4
    {"or r/m8, imm8",  {0x80}, 1, 1, 1, 8, -1, 1},      // /1
    {"xor r/m32, r32", {0x31}, 1, 1, 0, 32, -1, -1},
    {"test r/m64, r64",{0x85}, 1, 1, 0, 64, -1, -1},

    // shifts/rotates ops.
    {"shl r/m8, 1",  {0xD0}, 1, 1, 0, 8, -1, 4},   // /4
    {"shr r/m32, cl",{0xD3}, 1, 1, 0, 32, -1, 5},  // /5
    {"sar r/m64, imm8", {0xC1}, 1, 1, 1, 64, -1, 7}, // /7

    // control flow ops.
    {"jmp rel8",  {0xEB}, 1, 0, 1, 0, -1, -1},
    {"jmp rel32", {0xE9}, 1, 0, 4, 0, -1, -1},
    {"jmp r/m64", {0xFF}, 1, 1, 0, 64, -1, 4}, // /4
    {"call rel32",{0xE8}, 1, 0, 4, 0, -1, -1},
    {"call r/m64",{0xFF}, 1, 1, 0, 64, -1, 2}, // /2
    {"ret",       {0xC3}, 1, 0, 0, 0, -1, -1},
    {"ret imm16", {0xC2}, 1, 0, 2, 0, -1, -1},

    // conditional jumps (short rel8).
    {"jo rel8",   {0x70}, 1, 0, 1, 0, -1, -1},
    {"jno rel8",  {0x71}, 1, 0, 1, 0, -1, -1},
    {"jb rel8",   {0x72}, 1, 0, 1, 0, -1, -1}, // aka jc, jnae
    {"jnb rel8",  {0x73}, 1, 0, 1, 0, -1, -1}, // aka jnc, jae
    {"je rel8",   {0x74}, 1, 0, 1, 0, -1, -1}, // aka jz
    {"jne rel8",  {0x75}, 1, 0, 1, 0, -1, -1}, // aka jnz
    {"jbe rel8",  {0x76}, 1, 0, 1, 0, -1, -1}, // aka jna
    {"ja rel8",   {0x77}, 1, 0, 1, 0, -1, -1}, // aka jnbe
    {"js rel8",   {0x78}, 1, 0, 1, 0, -1, -1},
    {"jns rel8",  {0x79}, 1, 0, 1, 0, -1, -1},
    {"jp rel8",   {0x7A}, 1, 0, 1, 0, -1, -1}, // aka jpe
    {"jnp rel8",  {0x7B}, 1, 0, 1, 0, -1, -1}, // aka jpo
    {"jl rel8",   {0x7C}, 1, 0, 1, 0, -1, -1}, // aka jnge
    {"jge rel8",  {0x7D}, 1, 0, 1, 0, -1, -1}, // aka jnl
    {"jle rel8",  {0x7E}, 1, 0, 1, 0, -1, -1}, // aka jng
    {"jg rel8",   {0x7F}, 1, 0, 1, 0, -1, -1}, // aka jnle

    // conditional jumps (near rel32).
    {"jo rel32",   {0x0F,0x80}, 2, 0, 4, 0, -1, -1},
    {"jno rel32",  {0x0F,0x81}, 2, 0, 4, 0, -1, -1},
    {"jb rel32",   {0x0F,0x82}, 2, 0, 4, 0, -1, -1},
    {"jnb rel32",  {0x0F,0x83}, 2, 0, 4, 0, -1, -1},
    {"je rel32",   {0x0F,0x84}, 2, 0, 4, 0, -1, -1},
    {"jne rel32",  {0x0F,0x85}, 2, 0, 4, 0, -1, -1},
    {"jbe rel32",  {0x0F,0x86}, 2, 0, 4, 0, -1, -1},
    {"ja rel32",   {0x0F,0x87}, 2, 0, 4, 0, -1, -1},
    {"js rel32",   {0x0F,0x88}, 2, 0, 4, 0, -1, -1},
    {"jns rel32",  {0x0F,0x89}, 2, 0, 4, 0, -1, -1},
    {"jp rel32",   {0x0F,0x8A}, 2, 0, 4, 0, -1, -1},
    {"jnp rel32",  {0x0F,0x8B}, 2, 0, 4, 0, -1, -1},
    {"jl rel32",   {0x0F,0x8C}, 2, 0, 4, 0, -1, -1},
    {"jge rel32",  {0x0F,0x8D}, 2, 0, 4, 0, -1, -1},
    {"jle rel32",  {0x0F,0x8E}, 2, 0, 4, 0, -1, -1},
    {"jg rel32",   {0x0F,0x8F}, 2, 0, 4, 0, -1, -1},

    // stack/flags ops.
    {"enter", {0xC8}, 1, 0, 3, 0, -1, -1},
    {"leave", {0xC9}, 1, 0, 0, 0, -1, -1},
    {"pushf", {0x9C}, 1, 0, 0, 0, -1, -1},
    {"popf",  {0x9D}, 1, 0, 0, 0, -1, -1},
    {"clc",   {0xF8}, 1, 0, 0, 0, -1, -1},
    {"stc",   {0xF9}, 1, 0, 0, 0, -1, -1},
    {"cmc",   {0xF5}, 1, 0, 0, 0, -1, -1},

    // system/misc ops.
    {"int imm8", {0xCD}, 1, 0, 1, 0, -1, -1},
    {"int3",     {0xCC}, 1, 0, 0, 0, -1, -1},
    {"int1",     {0xF1}, 1, 0, 0, 0, -1, -1}, // icebp
    {"into",     {0xCE}, 1, 0, 0, 0, -1, -1},
    {"syscall",  {0x0F,0x05}, 2, 0, 0, 0, -1, -1},
    {"sysret",   {0x0F,0x07}, 2, 0, 0, 0, -1, -1},
    {"hlt",      {0xF4}, 1, 0, 0, 0, -1, -1},
    {"nop",      {0x90}, 1, 0, 0, 0, -1, -1},
    {"nop r/m16",{0x0F,0x1F}, 2, 1, 0, 16, -1, 0}, // /0 multi-byte nop
    {"ud2",      {0x0F,0x0B}, 2, 0, 0, 0, -1, -1},

    // loop family.
    {"loop rel8",   {0xE2}, 1, 0, 1, 0, -1, -1},
    {"loope rel8",  {0xE1}, 1, 0, 1, 0, -1, -1}, // aka loopz
    {"loopne rel8", {0xE0}, 1, 0, 1, 0, -1, -1}, // aka loopnz
    {"jecxz rel8",  {0xE3}, 1, 0, 1, 32, -1, -1},
    {"jrcxz rel8",  {0xE3}, 1, 0, 1, 64, -1, -1},

    // flag ops.
    {"lahf", {0x9F}, 1, 0, 0, 0, -1, -1},
    {"sahf", {0x9E}, 1, 0, 0, 0, -1, -1},
    {"seto r/m8",   {0x0F,0x90}, 2, 1, 0, 8, -1, 0},
    {"setno r/m8",  {0x0F,0x91}, 2, 1, 0, 8, -1, 1},
    {"setb r/m8",   {0x0F,0x92}, 2, 1, 0, 8, -1, 2},
    {"setnb r/m8",  {0x0F,0x93}, 2, 1, 0, 8, -1, 3},
    {"sete r/m8",   {0x0F,0x94}, 2, 1, 0, 8, -1, 4},
    {"setne r/m8",  {0x0F,0x95}, 2, 1, 0, 8, -1, 5},
    {"setbe r/m8",  {0x0F,0x96}, 2, 1, 0, 8, -1, 6},
    {"seta r/m8",   {0x0F,0x97}, 2, 1, 0, 8, -1, 7},
    {"sets r/m8",   {0x0F,0x98}, 2, 1, 0, 8, -1, 0},
    {"setns r/m8",  {0x0F,0x99}, 2, 1, 0, 8, -1, 1},
    {"setp r/m8",   {0x0F,0x9A}, 2, 1, 0, 8, -1, 2},
    {"setnp r/m8",  {0x0F,0x9B}, 2, 1, 0, 8, -1, 3},
    {"setl r/m8",   {0x0F,0x9C}, 2, 1, 0, 8, -1, 4},
    {"setge r/m8",  {0x0F,0x9D}, 2, 1, 0, 8, -1, 5},
    {"setle r/m8",  {0x0F,0x9E}, 2, 1, 0, 8, -1, 6},
    {"setg r/m8",   {0x0F,0x9F}, 2, 1, 0, 8, -1, 7},

    // system.
    {"cpuid", {0x0F,0xA2}, 2, 0, 0, 0, -1, -1},
    {"bt r/m32, r32", {0x0F,0xA3}, 2, 1, 0, 32, -1, -1},
    {"bt r/m64, r64", {0x0F,0xA3}, 2, 1, 0, 64, -1, -1},

    // cet / endbr.
    {"endbr32", {0xF3,0x0F,0x1E,0xFA}, 4, 0, 0, 32, -1, -1},
    {"endbr64", {0xF3,0x0F,0x1E,0xFB}, 4, 0, 0, 64, -1, -1},
};

#define RDA_AMD64_TABLE_SZ sizeof(table_amd64)/sizeof(i_amd64_t)
#endif //LRDA_AMD64_H
