#ifndef STRING_H
#define STRING_H

#include "utils.h"

typedef char* string_t;

typedef struct {
    string_t base;
    u64 size;
} String;

u16 string_len(String literal);
u16 parser_immediate(String src);
String new_string(string_t base, u64 size);
u8 string_equal_to(String src, string_t target_str);
u8 string_compare_to(String src, String target);

#endif
