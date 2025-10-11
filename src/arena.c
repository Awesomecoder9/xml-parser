#include "include/arena.h"
#include <stdlib.h>
#include <stdio.h>

void arena_init(arena_t *arena, const uint64_t size)
{
    *arena = (arena_t){
        .data = malloc(size),
        .offset = 0,
        .threshold = size};
}

void arena_reset(arena_t *arena)
{
    arena->offset = 0;
}

void arena_free(arena_t *arena)
{
    printf("Used %f%% of %ld KB", ((float)arena->offset / arena->threshold) * 100, arena->threshold / KB(1));
    free(arena->data);
    *arena = (arena_t){
        .data = NULL,
        .offset = 0,
        .threshold = 0};
}

void *arena_alloc(arena_t *arena, const uint64_t size, const uint64_t align)
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

void *arena_alloc_array(arena_t *arena, const uint64_t elemSize, const uint64_t numOfItems, const uint64_t align)
{
    const uint64_t totalSize = elemSize * numOfItems;
    if (totalSize < elemSize)
        return NULL;
    return arena_alloc(arena, totalSize, align);
}
