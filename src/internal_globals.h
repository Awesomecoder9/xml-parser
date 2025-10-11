#pragma once
#include <assert.h>
#define NEXUS_PARSER_API __attribute__((visibility("default")))

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

