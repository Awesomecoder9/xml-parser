#pragma once
#include <stdint.h>
#include <stdlib.h>
#define KB(x) (x*1000)
#define MB(x) (KB(x)*1000)
#define GB(x) (MB(x)*1000)
typedef struct
{
    uint64_t offset, threshold;
    void* data;
} arena_t;

void arena_init(arena_t* arena, const uint64_t size);
void arena_reset(arena_t* arena);
void arena_free(arena_t* arena);
void* arena_alloc(arena_t* arena, const uint64_t size, const uint64_t align);
void* arena_alloc_array(arena_t* arena, const uint64_t elemSize, const uint64_t numOfItems, const uint64_t align);


