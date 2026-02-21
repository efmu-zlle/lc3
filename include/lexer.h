#ifndef LEXER_H
#define LEXER_H

#include "base/arena.h"
#include "base/string.h"

#define MAX_TOKENS KiB(4)

#define CURRENT_TOKEN() \
    tokens.items[i];

#define NEXT_TOKEN() \
    tokens.items[++i];

typedef enum {
    OP_BR = 0,
    OP_ADD,
    OP_LD,
    OP_ST,
    OP_JSR,
    OP_AND,
    OP_LDR,
    OP_STR,
    OP_RTI,
    OP_NOT,
    OP_LDI,
    OP_STI,
    OP_JMP,
    OP_RES,
    OP_LEA,
    OP_TRAP,
    OP_NONE,
} OpcodeType;

typedef enum {
    TRAP_GETC = 0x20,
    TRAP_OUT = 0x021,
    TRAP_PUTS = 0x22,
    TRAP_IN = 0x23,
    TRAP_PUTSP = 0x24,
    TRAP_HALT = 0x25
} TrapType;

typedef enum {
    TOKEN_LABEL = 0x00,
    TOKEN_DIRECTIVE,
    TOKEN_OPCODE,
    TOKEN_OPERANDS,
    TOKEN_COMMA,
    TOKEN_EOL,
    TOKEN_EOF,
} TokenType;

typedef struct {
    OpcodeType type;
    string_t literal;
} Opcodes;

typedef struct {
    TrapType type;
    string_t literal;
} Trap;

typedef struct {
    TokenType type;
    OpcodeType opcode;
    String literal;
} Token;

typedef struct {
    Token* items;
    u32 size;
    u32 capacity;
} TokenList;

typedef struct {
    string_t input;
    u16 input_len;
    u16 read_position;
    u16 position;
    char ch;
} Lexer;

typedef enum {
    TOKENIZE_LINE_HAS_CONTENT = 1 << 0,
    TOKENIZE_IN_DIRECTIVE     = 1 << 1,
    TOKENIZE_ERROR            = 1 << 2 
} TokenizeState;

u8 peek_char(Lexer* l);
u8 is_custom_digit(Lexer* l);

void read_char(Lexer* l);
Lexer new_lexer(string_t input);
void skip_whitespace(Lexer *l);
Token new_token(String literal, TokenType type, OpcodeType opcode);

String new_string(string_t base, u64 size);
Token next_token(Lexer* l);
void token_list_add(TokenList* list, Token t);
TokenList tokenize(Arena* arena, string_t input);
void print_tokens(TokenList tokens);

#endif
