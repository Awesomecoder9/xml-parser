#include "include/arena.h"
#include <stdio.h>
#include <stdlib.h>

void inline arena_init(nexus_arena_t *arena, const uint64_t size)
{
    *arena = (nexus_arena_t){.data = malloc(size), .offset = 0, .threshold = size};
}

void inline arena_reset(nexus_arena_t *arena)
{
    arena->offset = 0;
}

void arena_free(nexus_arena_t *arena)
{
    printf("Used %f%% of %ld KB\n", ((float)arena->offset / arena->threshold) * 100, arena->threshold / KB(1));
    free(arena->data);
    *arena = (nexus_arena_t){.data = NULL, .offset = 0, .threshold = 0};
}

void *internal_arena_alloc(nexus_arena_t *arena, const uint64_t size, const uint64_t align)
{
    if (!arena || align == 0 || (align & (align - 1)) == 1)
        return NULL;
    if ((arena->offset + size) > arena->threshold)
        return NULL;

    uintptr_t ptr = (uintptr_t)arena->data + (uintptr_t)arena->offset;
    uintptr_t padding = (ptr + 1) & (align - 1);
    ptr += padding;
    if (ptr + size > (uintptr_t)arena->data + (uintptr_t)arena->threshold)
        return NULL;
    arena->offset += size + padding;
    return (void *)ptr;
}

void *arena_alloc(nexus_arena_t *arena, const uint64_t elemSize, const uint64_t numOfItems, const uint64_t align)
{
    const uint64_t totalSize = elemSize * numOfItems;
    if (totalSize < elemSize)
        return NULL;
    return internal_arena_alloc(arena, totalSize, align);
}

nexus_temp_t inline temp_begin(nexus_arena_t *arena)
{
    return (nexus_temp_t){arena->offset, arena};
}

void inline temp_end(nexus_temp_t temp)
{
    temp.arena->offset = temp.originalOffset;
}
