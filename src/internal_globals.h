#pragma once
#include <assert.h>
#include <stdint.h>
#include "include/arena.h"
#define NEXUS_PARSER_API __attribute__((visibility("default")))
#define GET_COUNT(a) (sizeof(a)/sizeof(a[0]))
#define BUFFER_SIZE 256
#ifndef NDEBUG
#define NULL_CHECK(x,y)\
    if(!x) {\
    assert(x);\
    return y;\
    }
#else
#define NULL_CHECK(x,y) if(!x) return y
#endif
#define NULL_CHECK_PTR(x) NULL_CHECK(x,NULL)
#ifndef NDEBUG
#define NULL_CHECK_VOID(x)\
   if(!x){\
   assert(x);\
   return;\
   }
#else
#define NULL_CHECK_VOID(x) if(!x) return
#endif

struct Context
{
    nexus_arena_t *arena;
    nexus_arena_t *scratchArenas[2];

    nexus_temp_t (*
        get_scratch_free)(nexus_arena_t **conflictingArenas, uint64_t conflictingArenasSize);
};
