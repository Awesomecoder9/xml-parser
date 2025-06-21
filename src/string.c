#include <stdlib.h>
#include "string.h"
#include <string.h>



void string_cat(string_t* dest,string_t* src){
    strcat(dest->value, src->value);
}

string_t* cstr_to_string(const char* str){
    int len = strlen(str);
    char* newValue = (char*)malloc(len);
    memmove(newValue, str, len);
    string_t* newStr = (string_t*)malloc(sizeof(string_t));
    newStr->length = len;
    newStr->value = newValue;
    return newStr;
};
