#include "../../include/base/string.h"

u16 string_len(String literal) {
    u16 len = 0;
    u16 i = 0;

    while (i < literal.size) {
        if (literal.base[i] == '\\') {
            i++;
            
            if (literal.base[i]) {
                i++;
            }
            
            len++;
        } else {
            i++;
            len++;
        }
    }
    return len + 1;
}

u16 parser_immediate(String src) {
    u16 number = 0, current = 0, i = 0;
    u8 neg = 0;

    if (src.base[i] == '#') { /* decimal */
        i++; /* skip '#' */
        if (src.base[i] == '-') {
            neg = 1;
            i++; /* skip '-' */
        }

        while (i < src.size) {
            char ch = src.base[i];
            if (ch >= '0' && ch <= '9')
                current = (ch - '0');

            number = number * 0x0a + current; /* NOTE: 0 = 0 * 10 + ej: 5 */
            i++;
        }

        if (neg) {
            number = ~number + 0x01;
        }

    } else if (src.base[i] == 'x') { /* hexadecimal */
        i++; /* skip 'x' */
        while (i < src.size) {
            char ch = src.base[i];
            if (ch >= '0' && ch <= '9')
                current = (ch - '0'); /* get value from 0 to 9 */

            if (ch >= 'A' && ch <= 'F')
                current = (ch - 'A') + 0x0a; /* ej: 68 - 65 + 10 */

            number = number * 0x10 + current;
            i++;
        }
    }

    return number;
}

String new_string(string_t base, u64 size) {
    return (String){.base = base, .size = size};
}

u8 string_equal_to(String src, string_t target_str) {
    size_t len = strlen(target_str);
    if (src.size != len) return 0;
    return memcmp(src.base, target_str, src.size) == 0;
}

u8 string_compare_to(String src, String target) {
    if (src.size != target.size) return 0;
    return memcmp(src.base, target.base, src.size) == 0;
}
