#include "../../include/base/arena.h"

Arena* arena_init(u64 capacity) {
    Arena* new_arena = (Arena*)malloc(capacity);

    new_arena->capacity = capacity;
    new_arena->offset = ARENA_BASE_SIZE;

    return new_arena;
}

void* arena_alloc(Arena* arena, u64 size, b8 zero) {
    u64 new_offset = ALIGN_UP_POW2(arena->offset, ALIGN_BASE);
    u64 next_offset = new_offset + size;

    if (next_offset > arena->capacity) return NULL;

    arena->offset = next_offset;

    /* casting u8 so I can get exactly the memory 
    I want and it doesn't get disturbed because 
    of the (Arena) */
    u8* memory = &((u8*)arena)[new_offset];

    if (zero) {
        memset(memory, 0, size);
    }

    return memory;
}

void arena_dealloc(Arena* arena, u64 bytes) {
    bytes = MIN(bytes, arena->offset - ARENA_BASE_SIZE);
    arena->offset -= bytes;
}

void arena_dealloc_to(Arena* arena, u64 offset) {
    u64 bytes = offset < arena->offset ? arena->offset - offset : 0; 
    arena_dealloc(arena, bytes);
}

void arena_reset(Arena* arena) {
    arena_dealloc_to(arena, ARENA_BASE_SIZE);
}

void arena_free(Arena** arena) {
    free(*arena);
    *arena = NULL;
}
