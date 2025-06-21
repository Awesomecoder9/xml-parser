#pragma once

typedef struct {
    int length;
    char* value;
}string_t;


void string_cat(string_t*,string_t*);
string_t* cstr_to_string(const char*);