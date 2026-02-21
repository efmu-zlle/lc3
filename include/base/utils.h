#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>

typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;

typedef int64_t s64;
typedef int32_t s32;
typedef int16_t s16;
typedef int8_t s8;

typedef u8 b8;

#define KiB(n) (n << 10)
#define MiB(n) (n << 20)
#define GiB(n) (n << 30)

#define MIN(a, b) ((a < b) ? a : b)
#define MAX(a, b) ((a > b) ? a : b)

#define ALIGN_BASE sizeof(void*)
#define ALIGN_UP_POW2(n, p) (((u64)n + ((u64)p - 1)) & (~((u64)p - 1)))

u8 is_letter(char ch);
u8 is_digit(char ch);
u16 swap16(u16 a);

#endif
