#include <stdlib.h>
#include "string.h"
#include <string.h>

#define IS_CONT_BYTE(x) ((x & 0xC0) == 0x80)


void string_cat(string_t* dest,string_t* src){
    strncat(dest->value, src->value, dest->length);
}

string_t* cstr_to_string(const char* str){
    int len = strlen(str);
    char* newValue = malloc(len);
    memmove(newValue, str, len);
    string_t* newStr = malloc(sizeof(string_t));
    newStr->length = len;
    newStr->value = newValue;
    return newStr;
}
uint32_t utf8_parse(const char *str, uint32_t *out)
{
    // Normal ASCII
    if ((str[0] & 0x80) == 0)
    {
        *out = str[0];
        return 1;
    }
    else if ((str[0] & 0xE0) == 0xC0)
    {
        if (!IS_CONT_BYTE(str[1])) return -1;
        *out = (str[0] & 0x1F) << 6 | (str[1] & 0x3F);
        return 2;
    }
    else if ((str[0] & 0xF0) == 0xE0)
    {
        if ( !IS_CONT_BYTE(str[1]) || !IS_CONT_BYTE(str[2]) ) return -1;
        *out = (str[0] & 0x0F) << 12 | (str[1] & 0x3F) << 6 | (str[2] & 0x3F);
        return 3;
    }
    else if ((str[0] & 0xF8) == 0xF0)
    {
        if ( !IS_CONT_BYTE(str[1])|| !IS_CONT_BYTE(str[2]) || !IS_CONT_BYTE(str[3]) ) return -1;
        *out = (str[0] & 0x07) << 18 | (str[1] & 0x3F) << 12 | (str[2] & 0x3F) << 6 | (str[3] & 0x3F);
        return 4;
    }
    return -1;
};
