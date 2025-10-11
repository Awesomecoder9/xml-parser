#pragma once
#include "include/string.h"

#include <stdint.h>

struct Global
{
    void *(*allocate)(uint64_t numOfItems, uint64_t elemSize, uint64_t align);

    void (*deallocate)(void *);
};


uint64_t internal_strlen(const char *);

/**
 * Parses a valid UTF-8 character and puts the codepoint in the out variable
 * @params
 * str - Valid UTF-8 character
 * out - valid pointer to store the decoded codepoint
 * @returns number of bytes used in the encoding, -1 if character invalid
 */
uint32_t utf8_parse(const char *str, uint32_t *out);

void nexus_internal_string_init(void *(*allocate)(uint64_t, uint64_t, uint64_t), void (*deallocate)(void *));

uint64_t *nexus_string_mut_used(const string_t *str);