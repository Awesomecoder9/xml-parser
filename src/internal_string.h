#pragma once
#include "include/string.h"

#include <stdbool.h>
#include <stdint.h>


uint64_t nexus_internal_strlen(const char *);

/**
 * Parses a valid UTF-8 character and puts the parsed codepoint in `out`.
 *
 * @param str A pointer to a valid UTF-8 character
 * @param out a pointer to store the decoded codepoint
 * @returns number of bytes used in the encoding, -1 if the character is invalid.
 */
uint32_t nexus_internal_utf8_parse(const char *str, uint32_t *out);

bool nexus_internal_utf8_char_cmp(const char *a, const char *b);

bool is_all_whitespace(const string_t *str);