#ifndef SYMBOL_H
#define SYMBOL_H

#include "base/arena.h"
#include "lexer.h"

#define MAX_SYMBOLS KiB(4)

typedef struct {
    String literal;
    u16 address;
} Symbol;

typedef struct {
    Symbol* items;
    u32 count;
    u16 origin;
} SymbolTable;

typedef enum {
    DIR_ORIG, 
    DIR_FILL,
    DIR_BLKW,
    DIR_STRINGZ,
    DIR_NONE
} DirectiveType;

typedef struct {
    DirectiveType type;
    string_t literal;
} Directive;

u16 get_address(SymbolTable table, String literal);
DirectiveType get_directive(String literal);
SymbolTable build_sym_table(Arena* arena, TokenList tokens);
void print_sym_table(SymbolTable table);

#endif
