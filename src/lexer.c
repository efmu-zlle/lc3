#include "../include/lexer.h"

Opcodes literal_opcodes[] = {
    { OP_BR, "BR" },
    { OP_ADD, "ADD" },
    { OP_LD, "LD" },
    { OP_ST, "ST" },
    { OP_JSR, "JSR" },
    { OP_AND, "AND" },
    { OP_LDR, "LDR" },
    { OP_STR, "STR" },
    { OP_RTI, "RTI" },
    { OP_NOT, "NOT" },
    { OP_LDI, "LDI" },
    { OP_STI, "STI" },
    { OP_JMP, "JMP" },
    { OP_RES, "RES" },
    { OP_LEA, "LEA" },
    { OP_TRAP, "TRAP" }
};

Trap tp[] = {
    { TRAP_GETC, "GETC" },
    { TRAP_OUT, "OUT" },
    { TRAP_PUTS, "PUTS" },
    { TRAP_IN, "IN" },
    { TRAP_PUTSP, "PUTSP" },
    { TRAP_HALT, "HALT" }
};

u8 peek_char(Lexer* l) {
    if (l->read_position >= l->input_len) {
        return '\0';
    } else {
        return l->input[l->read_position];
    }
}

void read_char(Lexer* l) {
    if (l->read_position >= l->input_len) {
        l->ch = '\0';
    } else {
        l->ch = l->input[l->read_position];
    }
    l->position = l->read_position;
    l->read_position += 1;
}

void skip_whitespace(Lexer *l) {
    while (l->ch != '\0') {
        if (l->ch == '\t' || l->ch == '\r' || l->ch == ' ') {
            read_char(l);
        } else if (l->ch == ';') {
            while (l->ch != '\n' && l->ch != '\0') read_char(l);
        } else {
            break;
        }
    }
}

Lexer new_lexer(string_t input) {
    Lexer l = { .ch = '\0', .input = input, .read_position = 0, .position = 0, .input_len = strlen(input) };
    read_char(&l);
    return l;
}

Token new_token(String literal, TokenType type, OpcodeType opcode) {
    return (Token){.literal = literal, .type = type, .opcode = opcode};
}

OpcodeType is_opcode(String src) {
    if (src.size >= 2 && src.base[0] == 'B' && src.base[1] == 'R') {
        bool valid_br = true;
        for (u16 i = 2; i < src.size; i++) {
            char c = src.base[i];
            if (c != 'n' && c != 'z' && c != 'p') {
                valid_br = false;
                break;
            }
        }
        if (valid_br) return OP_BR;
    }

    if (string_equal_to(src, "RET")) return OP_JMP;

    u16 trap_count = sizeof(tp) / sizeof(tp[0]);
    for (u16 i = 0; i < trap_count; i++) {
        if (string_equal_to(src, tp[i].literal)) return OP_TRAP;
    }

    u16 op_count = sizeof(literal_opcodes) / sizeof(literal_opcodes[0]);
    for (u16 i = 0; i < op_count; i++) {
        if (string_equal_to(src, literal_opcodes[i].literal)) {
            return literal_opcodes[i].type;
        }
    }

    return OP_NONE;
}

Token next_token(Lexer* l) {
    skip_whitespace(l);
    Token token = {0};
    u16 pos = l->position;

    switch (l->ch) {
        case ',':
            read_char(l);
            return new_token(new_string(l->input + pos, 1), TOKEN_COMMA, OP_NONE);

        case '\"': {
            read_char(l);
            pos = l->position; 
            while (l->ch != '\0' && l->ch != '\"') read_char(l);
            u16 size = l->position - pos;
            token = new_token(new_string(l->input + pos, size), TOKEN_OPERANDS, OP_NONE);
            if (l->ch == '\"') read_char(l);
            return token;
        } break;
        
        case '\n': 
            read_char(l);
            return new_token(new_string(l->input + pos, 1), TOKEN_EOL, OP_NONE);

        case '\0': return new_token(new_string(l->input + pos, 0), TOKEN_EOF, OP_NONE);

        default: {
            TokenType token_type = TOKEN_LABEL;
            if (l->ch == '.') token_type = TOKEN_DIRECTIVE;
            else if (l->ch == '#' || l->ch == 'x' || (l->ch == 'R' && is_digit(peek_char(l)))) token_type = TOKEN_OPERANDS;

            while (l->ch != '\0' && l->ch != ' ' && l->ch != ',' && l->ch != '\n' && l->ch != ';') {
                read_char(l);
            }

            u16 size = l->position - pos;
            String str = new_string(l->input + pos, size);
            OpcodeType code_type = is_opcode(str);
            if (code_type != OP_NONE) {
                token = new_token(str, TOKEN_OPCODE, code_type);
            } else {
                token = new_token(str, token_type, OP_NONE);
            }
        } break;
    }
    return token;
}

void token_list_add(TokenList* list, Token t) {
    if (list->size < list->capacity) {
        list->items[list->size++] = t;
    } else {
        fprintf(stderr, "Error: Token list capacity exceeded!\n");
    }
}

TokenList tokenize(Arena* arena, string_t input) {
    TokenList list = {0};
    list.capacity = MAX_TOKENS;
    list.items = ARRAY_ALLOC(arena, Token, list.capacity);

    Token t = {0};
    Lexer l = new_lexer(input);
    u8 flags = 0;

    do {
        t = next_token(&l);
        
        if (t.type == TOKEN_EOL) {
            if (flags & TOKENIZE_LINE_HAS_CONTENT) {
                token_list_add(&list, t);
                flags &= ~TOKENIZE_LINE_HAS_CONTENT;
            }
            continue;
        }

        if (t.type == TOKEN_COMMA) {
            // flags |= TOKENIZE_LINE_HAS_CONTENT;
            continue;
        }

        if (t.type == TOKEN_DIRECTIVE || t.type == TOKEN_OPCODE || t.type == TOKEN_LABEL) {
            flags |= TOKENIZE_LINE_HAS_CONTENT;
        }

        token_list_add(&list, t);
    } while(t.type != TOKEN_EOF);

    return list;
}

void print_tokens(TokenList tokens) {
    for (u32 i = 0; i < tokens.size; i++) {
        Token t = tokens.items[i];
        printf("{Lexeme: '%.*s', Address: %p}", (s32)t.literal.size, t.literal.base, t.literal.base);
    }
}
