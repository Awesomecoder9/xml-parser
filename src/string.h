#pragma once
#include <stdint.h>
typedef struct {
    int length;
    char* value;
}string_t;


void string_cat(string_t*,string_t*);
string_t* cstr_to_string(const char*);
/*
 * Parses a valid UTF-8 character and puts the codepoint in the out variable
 * @params
 * str - Valid UTF-8 character
 * out - valid pointer to store the decoded codepoint
 * @returns number of bytes used in the encoding, -1 if character invalid
 */
uint32_t utf8_parse(const char* str, uint32_t* out);
