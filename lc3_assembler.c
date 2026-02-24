#include "src/base/utils.c"
#include "src/base/string.c"
#include "src/base/arena.c"
#include "src/lexer.c"
#include "src/symbol.c"
#include "src/assemble.c"

string_t get_input(Arena* arena, string_t filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    string_t buffer = ARRAY_ALLOC(arena, char, size + 1); /* size + 1 for '\0' */
    if (fread(buffer, sizeof(char), size, file) > 0) {
        buffer[size] = '\0';
    }

    fclose(file);
    return buffer;
}

void write_output_file(BinaryImage image) {
    FILE *file = fopen("asm/my_output.obj", "wb");

    u16 origin_be = swap16(image.origin);
    fwrite(&origin_be, sizeof(u16), 1, file);

    for (u32 i = 0; i < image.size; i++) {
        u16 word = swap16(image.data[i]);
        fwrite(&word, sizeof(u16), 1, file);
    }

    fclose(file);
}

s32 main(s32 argc, string_t argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Error: missing asm/<file>.asm...\n");
        exit(EXIT_FAILURE);
    }

    Arena* perm_arena = arena_init(KiB(256));

    /* 1. Fetch: File -> Buffer */
    string_t input = get_input(perm_arena, argv[1]);
    if (!input) {
        printf("failed to load image: %s\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    /* 2. Tokenization: Buffer -> TokenList */
    TokenList tokens = tokenize(perm_arena, input);
    
    /* 3. Create Sym Table: TokenList -> SymbolTable */
    SymbolTable table = build_sym_table(perm_arena, tokens);

    /* 4. Bytecode: TokenList + SymbolTable */
    BinaryImage image = assemble(perm_arena, tokens, table);

    /* 5. Output: BinaryImage -> OutputFile */
    write_output_file(image);

    printf("Compilation: %d tokens, %d symbols.\n", tokens.size, table.count);
    printf("Arena: %zu capacity, %zu bytes used.\n", perm_arena->capacity, perm_arena->offset);

    arena_free(&perm_arena);
    return EXIT_SUCCESS;
}
