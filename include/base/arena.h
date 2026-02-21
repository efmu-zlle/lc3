#ifndef ARENA_H
#define ARENA_H

#include "utils.h"

typedef struct {
    u64 capacity;
    u64 offset;
} Arena;

#define ARENA_BASE_SIZE sizeof(Arena)

/* using heap at the moment, but could use the stack if we want it to */
/* #define ARENA_STACK(name, initial_size) \
    u8 name##_memory[initial_size]; \
    Arena* name = arena_init(name##_memory, initial_size); */

#define STRUC_ALLOC(arena, type) \
    (type*)arena_alloc(arena, sizeof(type), false)

#define STRUC_ALLOC_Z(arena, type) \
    (type*)arena_alloc(arena, sizeof(type), true)

#define ARRAY_ALLOC(arena, type, n) \
    (type*)arena_alloc(arena, sizeof(type) * n, false)

#define ARRAY_ALLOC_Z(arena, type, n) \
    (type*)arena_alloc(arena, sizeof(type) * n, true)

Arena* arena_init(u64 capacity);
void* arena_alloc(Arena* arena, u64 size, b8 zero);
void arena_dealloc(Arena* arena, u64 bytes);
void arena_dealloc_to(Arena* arena, u64 offset);
void arena_reset(Arena* arena);
void arena_free(Arena** arena);

#endif
