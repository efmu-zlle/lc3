#include "../../include/base/utils.h"

u8 is_letter(char ch) {
    return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z');
}

u8 is_digit(char ch) {
   return (ch >= '0' && ch <= '9');
}

u16 swap16(u16 a) {
    return (a << 8) | (a >> 8);
}
