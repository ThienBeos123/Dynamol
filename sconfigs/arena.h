#ifndef __DNML_ARENA_H__
#define __DNML_ARENA_H__

#include <stdint.h>
#include <stddef.h>
#include <stdalign.h>
#include <stdlib.h>
#include "../system/compiler.h"

//* ============= Declarations =============
typedef uint64_t max_align_t;
typedef struct {
    uint8_t *base;
    size_t  cap;
    size_t  offset;
} dnml_arena;

// Context Container for 
// nested operation independency
typedef struct {
    dnml_arena arena;
} dnml_ctx;

//* ============= Functionalities =============
static inline void init_arena(dnml_arena *a, size_t init_cap) {
    if (a->base) return;
    a->base = malloc(init_cap);
    if (!a->base) abort();
    a->cap      = init_cap;
    a->offset   = 0;
}
static inline void arena_destruct(dnml_arena *a) {
    if (!a->base) return;
    a->offset = 0;
    free(a->base);
    a->base     = NULL;
    a->cap      = 0;
    a->offset   = 0;
}
static inline size_t align_forward(size_t x, size_t align) { return (x + (align - 1)) & ~(align - 1); }
static inline void arena_grow(dnml_arena *a, size_t min_cap) {
    if (a->cap >= min_cap) return;
    size_t new_cap = (a->cap) ? a->cap : 1;
    while (new_cap < min_cap) new_cap *= 2;
    
    uint8_t *buf = realloc(a->base, new_cap);
    if (!buf) abort();
    a->base     = buf;
    a->cap      = new_cap; 
}

// Accepts any type (CRUCIAL)
static inline void *arena_alloc(dnml_arena *a, size_t space) {
    size_t align = alignof(max_align_t);
    size_t aligned_offset = align_forward(a->offset, align);
    size_t new_offset = aligned_offset + space;

    arena_grow(a, new_offset);
    void *ptr = a->base + aligned_offset;
    a->offset = new_offset;
    return ptr;
}
static inline size_t arena_mark(dnml_arena *a) { return a->offset; }
static inline void arena_reset(dnml_arena *a, size_t mark) { if (mark <= a->offset) a->offset = mark; }
static inline void arena_wipe(dnml_arena *a) { a->offset = 0; }

#endif