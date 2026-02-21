#include "../include/symbol.h"

Directive directives[] = {
    { DIR_ORIG, ".ORIG" }, 
    { DIR_FILL, ".FILL" },
    { DIR_BLKW, ".BLKW" },
    { DIR_STRINGZ, ".STRINGZ" }
};

u16 get_address(SymbolTable table, String literal) {
    for (u16 i = 0; i < table.count; i++) {
        if (string_compare_to(literal, table.items[i].literal))
            return table.items[i].address;
    }

    return 0;
}

DirectiveType get_directive(String literal) {
    size_t size = sizeof(directives) / sizeof(directives[0]);
    for (size_t i = 0; i < size; i++) {
        if (string_equal_to(literal, directives[i].literal)) {
            return directives[i].type;
        }
    }

    return DIR_NONE;
}

SymbolTable build_sym_table(Arena* arena, TokenList tokens) {
    SymbolTable table = {0};
    table.items = ARRAY_ALLOC(arena, Symbol, MAX_SYMBOLS);

    u16 current_pc = 0;
    u8 flags = 0;

    for (u32 i = 0; i < tokens.size; i++) {
        Token t = tokens.items[i];

        switch (t.type) {
            case TOKEN_LABEL:
                if (!(flags & TOKENIZE_LINE_HAS_CONTENT)) {
                    if (get_address(table, t.literal) == 0) {
                        if (table.count < MAX_SYMBOLS) {
                            table.items[table.count++] = (Symbol){ .literal = t.literal, .address = current_pc };
                        } else {
                            fprintf(stderr, "Sym Table: space exceeded!\n");
                        }
                    }
                }
                break;

            case TOKEN_OPCODE:
                current_pc += 1;
                flags |= TOKENIZE_LINE_HAS_CONTENT;
                break;

            case TOKEN_DIRECTIVE:
                flags |= TOKENIZE_LINE_HAS_CONTENT;
                DirectiveType dir = get_directive(t.literal);
                if (dir == DIR_ORIG) {
                    i++;
                    table.origin = parser_immediate(tokens.items[i].literal);
                    current_pc = table.origin;
                } else if (dir == DIR_FILL) {
                    current_pc += 1;
                } else if (dir == DIR_BLKW) {
                    i++;
                    current_pc += parser_immediate(tokens.items[i].literal);
                } else if (dir == DIR_STRINGZ) {
                    i++;
                    current_pc += string_len(tokens.items[i].literal);
                }
                break;

            case TOKEN_EOL:
                flags = 0;
                break;

            default: break;
        }
    }
    return table;
}

void print_sym_table(SymbolTable table) {
    for (u32 i = 0; i < table.count; i++) {
        Symbol s = table.items[i];
        printf("{ %.*s, %04X }\n", (s32)s.literal.size, s.literal.base, s.address);
    }
}

