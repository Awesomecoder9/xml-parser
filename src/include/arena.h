#pragma once
#include <stdint.h>

#define KB(x) (x*1000)
#define MB(x) (KB(x)*1000)
#define GB(x) (MB(x)*1000)

typedef struct {
    uint64_t offset, threshold;
    void *data;
} nexus_arena_t;

typedef struct {
    uint64_t originalOffset;
    nexus_arena_t *arena;
} nexus_temp_t;

void arena_init(nexus_arena_t *arena, uint64_t size);

void arena_reset(nexus_arena_t *arena);

void arena_free(nexus_arena_t *arena);

void *arena_alloc(nexus_arena_t *arena, uint64_t elemSize, uint64_t numOfItems, uint64_t align);

nexus_temp_t temp_begin(nexus_arena_t *arena);

void temp_end(nexus_temp_t temp);

#define new(a,type,n) arena_alloc(a,sizeof(type),n,_Alignof(type))
