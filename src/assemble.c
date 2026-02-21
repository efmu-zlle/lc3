#include "../include/assemble.h"

void add_binary_image(BinaryImage* image, u16 instruction) {
    if (image->size < MAX_BINARY_IMAGE) {
        image->data[image->size++] = instruction;
    } else {
        fprintf(stderr, "Error: Instruction capacity exceeded!\n");
    }
}

void handle_string(BinaryImage* image, String literal) {
    u16 i = 0;
    while (i < literal.size) {
        if (literal.base[i] == '\\') {
            i++;
            switch (literal.base[i]) {
                case 'n': add_binary_image(image, 0x000A); break;
                case 't': add_binary_image(image, 0x0009); break;
                case 'e': add_binary_image(image, 0x001B); break;
                case '"': add_binary_image(image, 0x0022); break;
                case '\\': add_binary_image(image, 0x005C); break;
                default:  add_binary_image(image, (u16)literal.base[i]); break;
            }
        } else {
            add_binary_image(image, (u16)literal.base[i]);
        }
        if (literal.base[i]) i++;
    }
    add_binary_image(image, 0x0000);
}

u16 pack_instruction(Token t, TokenList tokens, u32* index, SymbolTable table, u16 current_pc) {
    u16 instruction = 0;
    u32 i = *index;

    switch (t.opcode) {
        case OP_ADD: {
            Token dr_tok = NEXT_TOKEN();
            Token sr1_tok = NEXT_TOKEN();
            Token imm5_tok = NEXT_TOKEN();

            u16 dr = dr_tok.literal.base[1] - '0',
                sr1 = sr1_tok.literal.base[1] - '0';

            instruction = (OP_ADD << 12) | (dr << 9) | (sr1 << 6);  /* NOTE: (0000 0000 0000 0000 | 0001 0010 0100 0000) */
            char first_char = imm5_tok.literal.base[0];

            if (first_char == '#' || first_char == 'x') {
                u16 imm5 = parser_immediate(imm5_tok.literal);
                instruction|= (1 << 5); /* NOTE: bit [5] = 1         (0001 0010 0100 0000 | 0000 0000 0010 0000) */
                instruction |= (imm5 & 0x1f);   /* NOTE: bit [4...0] = imm5 (0 0000 & 1 1111) */
            } else {
                /* bit[5...3] is already 0 */
                u16 sr2 = imm5_tok.literal.base[1] - '0';
                instruction|= sr2; /* NOTE: [2...0]; */
            }
        } break;
        case OP_AND: {
            Token dr_tok = NEXT_TOKEN();
            Token sr1_tok = NEXT_TOKEN();
            Token imm5_tok = NEXT_TOKEN();

            u16 dr = dr_tok.literal.base[1] - '0',
                sr1 = sr1_tok.literal.base[1] - '0';

            instruction = (OP_AND << 12) | (dr << 9) | (sr1 << 6);

            char first_char = imm5_tok.literal.base[0];

            if (first_char == '#' || first_char == 'x') {
                u16 imm5 = parser_immediate(imm5_tok.literal);
                instruction |= (1 << 5);
                instruction |= (imm5 & 0x1f);
            } else {
                u16 sr2 = imm5_tok.literal.base[1] - '0';
                instruction |= (sr2 & 0x07);
            }
        } break;
        case OP_BR: {
                Token pc_offset9_tok = NEXT_TOKEN();
                u16 pc_offset9;

                char first = pc_offset9_tok.literal.base[0];
                if (first == '#' || first == 'x') {
                    pc_offset9 = parser_immediate(pc_offset9_tok.literal);
                } else {
                    u16 address = get_address(table, pc_offset9_tok.literal);
                    pc_offset9 = address - current_pc; 
                }

                instruction = (OP_BR << 12);

                u16 k = 2;
                if (t.literal.base[k] == ' ') {
                    instruction |= (7 << 9);
                } else {
                    while (k < t.literal.size) {
                        if (t.literal.base[k] == 'n') instruction |= (1 << 11);
                        if (t.literal.base[k] == 'z') instruction |= (1 << 10);
                        if (t.literal.base[k] == 'p') instruction |= (1 << 9);
                        k++;
                    }
                }

                instruction |= (pc_offset9 & 0x1ff);
        } break;   
        case OP_LD: {
            Token dr_tok = NEXT_TOKEN();
            Token pc_offset9_tok = NEXT_TOKEN();

            u16 dr = dr_tok.literal.base[1] - '0';
            u16 pc_offset9 = get_address(table, pc_offset9_tok.literal) - current_pc;

            instruction = (OP_LD << 12) | (dr << 9) | (pc_offset9 & 0x1ff);
        } break;
        case OP_ST: {
            Token sr_tok = NEXT_TOKEN();
            Token pc_offset9_tok = NEXT_TOKEN();

            u16 sr = sr_tok.literal.base[1] - '0';
            u16 address = get_address(table, pc_offset9_tok.literal);
            u16 pc_offset9 = address - current_pc;

            instruction = (OP_ST << 12) | (sr << 9) | (pc_offset9 & 0x1ff);
        } break;
        case OP_JSR: {
            Token pc_offset11_tok = NEXT_TOKEN();  
            u16 address = get_address(table, pc_offset11_tok.literal);
            u16 pc_offset11 = address - current_pc;

            instruction = (OP_JSR << 12) | (1 << 11) | (pc_offset11 & 0x7ff);
        } break;
        case OP_STR: {
            Token sr_tok = NEXT_TOKEN();
            Token base_r_tok = NEXT_TOKEN();
            Token offset6_tok = NEXT_TOKEN();

            u16 sr = sr_tok.literal.base[1] - '0';
            u16 base_r = base_r_tok.literal.base[1] - '0';
            u16 offset6 = parser_immediate(offset6_tok.literal);

            instruction = (OP_STR << 12) | (sr << 9) | (base_r << 6)| (offset6 & 0x3f);
        } break;
        case OP_LDR: {
            Token sr_tok = NEXT_TOKEN();
            Token base_r_tok = NEXT_TOKEN();
            Token offset6_tok = NEXT_TOKEN();

            u16 sr = sr_tok.literal.base[1] - '0';
            u16 base_r = base_r_tok.literal.base[1] - '0';
            u16 offset6 = parser_immediate(offset6_tok.literal);

            instruction = (OP_LDR << 12) | (sr << 9) | (base_r << 6)| (offset6 & 0x3f);
        } break;
        case OP_LDI: {
            Token dr_tok = NEXT_TOKEN();
            Token pc_offset9_tok = NEXT_TOKEN();

            u16 dr = dr_tok.literal.base[1] - '0';
            u16 pc_offset9 = get_address(table, pc_offset9_tok.literal) - current_pc;

            instruction = (OP_LDI << 12) | (dr << 9) | (pc_offset9 & 0x1ff);
        } break;
        case OP_STI: {
            Token sr_tok = NEXT_TOKEN();
            Token pc_offset9_tok = NEXT_TOKEN();

            u16 sr = sr_tok.literal.base[1] - '0';
            u16 address = get_address(table, pc_offset9_tok.literal);
            u16 pc_offset9 = address - current_pc;

            instruction = (OP_STI << 12) | (sr << 9) | (pc_offset9 & 0x1ff);
        } break;
        case OP_NOT: {
            Token dr_tok = NEXT_TOKEN();
            Token sr_tok = NEXT_TOKEN();

            u16 dr = dr_tok.literal.base[1] - '0';
            u16 sr = sr_tok.literal.base[1] - '0';

            instruction = (OP_NOT << 12) | (dr << 9) | (sr << 6) | (0x3f);
        } break;
        case OP_JMP: {
            instruction = OP_JMP << 12;
            if (string_equal_to(t.literal, "JMP")) {
                Token base_r_tok = NEXT_TOKEN();

                u16 base_r = base_r_tok.literal.base[1] - '0';
                instruction |= (base_r << 6);
            }
            
            if (string_equal_to(t.literal, "RET")) {
                instruction |= (7 << 6);                
            }
        } break;
            /* OP_RTI,
            OP_RES, */
        case OP_LEA: {
            Token sr_tok = NEXT_TOKEN();
            Token pc_offset9_tok = NEXT_TOKEN();

            u16 sr = sr_tok.literal.base[1] - '0';
            u16 address = get_address(table, pc_offset9_tok.literal);
            u16 pc_offset9 = address - current_pc;

            instruction = (OP_LEA << 12) | (sr << 9) | (pc_offset9 & 0x1ff); 
        } break;
        case OP_TRAP: {
            instruction = (OP_TRAP << 12);

            if (string_equal_to(t.literal, "TRAP")) { /* NOTE: we take the next token ej. TRAP x25 <- this */
                Token operand_tok = NEXT_TOKEN();
                u16 trapvec8 = parser_immediate(operand_tok.literal);
                instruction |= trapvec8;
            } else {
                u16 size = sizeof(tp) / sizeof(tp[0]);
                for (u16 pos = 0; pos < size; pos += 1) {
                    if (string_equal_to(t.literal, tp[pos].literal)) {
                        instruction |= tp[pos].type;
                        break;
                    }
                }
            }
        } break;
        default: break;
    }

    *index = i;
    return instruction;
}

BinaryImage assemble(Arena* arena, TokenList tokens, SymbolTable table) {
    BinaryImage image = {0};
    image.data = ARRAY_ALLOC(arena, u16, MAX_BINARY_IMAGE);
    image.origin = table.origin;

    u16 current_pc = image.origin;

    for (u32 i = 0; i < tokens.size; i++) {
        Token t = tokens.items[i];
        Token next_t;

        if (t.type == TOKEN_OPCODE) {
            u16 instruction = pack_instruction(t, tokens, &i, table, current_pc + 1);
            add_binary_image(&image, instruction);
            current_pc += 1;
        } else if (t.type == TOKEN_DIRECTIVE) {
            switch (get_directive(t.literal)) {
                case DIR_ORIG: {
                    i++;
                } continue;
                case DIR_FILL: {
                    next_t = NEXT_TOKEN()
                    u16 instruction;

                    char first = next_t.literal.base[0];
                    if (first == 'x' || first == '#') {
                        instruction = parser_immediate(next_t.literal);
                    } else {
                        instruction = get_address(table, next_t.literal);
                    }

                    add_binary_image(&image, instruction);
                    current_pc += 1;
                } continue;
                case DIR_BLKW: {
                    Token blkw_tok = NEXT_TOKEN();
                    u16 blkw = parser_immediate(blkw_tok.literal);
                    for (u16 b = 0; b < blkw; b++) {
                        add_binary_image(&image, 0);
                    }
                    current_pc += blkw;
                } continue;
                case DIR_STRINGZ: {
                    Token stringz_tok = NEXT_TOKEN();
                    u16 len = string_len(stringz_tok.literal);
                    handle_string(&image, stringz_tok.literal);
                    current_pc += len;
                } continue;
                default: break;
            }
        }
    }

    return image;
}
