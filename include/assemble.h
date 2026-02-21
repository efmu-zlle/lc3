#ifndef ASSEMBLE_H
#define ASSEMBLE_H

#include "base/arena.h"
#include "lexer.h"
#include "symbol.h"

#define MAX_BINARY_IMAGE KiB(4)

typedef struct {
    u16* data;
    u32 size;
    u16 origin; 
} BinaryImage;

void add_binary_image(BinaryImage* image, u16 instruction);
void handle_string(BinaryImage* image, String literal);
u16 pack_instruction(Token t, TokenList tokens, u32* index, SymbolTable table, u16 current_pc);
BinaryImage assemble(Arena* arena, TokenList tokens, SymbolTable table);

#endif
