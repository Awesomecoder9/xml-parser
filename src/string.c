#include <stdlib.h>
#include "include/string.h"
#include "internal_string.h"
#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>

/**
 * @brief Represents a dynamically managed string.
 *
 * This structure holds the necessary information for a variable-length string.
 */
struct i_string_t
{
    uint64_t length; /**< The total allocated capacity of the internal buffer. */
    uint64_t used; /**< The current number of characters/bytes stored in the string. */
    char *data; /**< Pointer to the underlying character array (buffer). */
    bool owned;
};

#define IS_CONT_BYTE(x) ((x & 0xC0) == 0x80)
#define IS_OWNED(x) if(!x->owned) return NULL;
struct Global globalState;

string_t *nexus_string_cat(string_t *dest, const string_t *src)
{
    NULL_CHECK_PTR(dest);
    NULL_CHECK_PTR(src);
    IS_OWNED(dest);
    const uint64_t newUsed = dest->used + src->used;
    if (newUsed > dest->length)
    {
        char *newData = globalState.allocate(newUsed, sizeof(char), __alignof(char));
        nexus_memcpy(newData, dest->data, dest->used);
        dest->data = newData;
        dest->length = newUsed;
        dest->used = newUsed - src->used;
    }
    string_t sub = (string_t){dest->length - dest->used, 0, &dest->data[dest->used]};
    nexus_string_cpy(&sub, src);
    return dest;
}

string_t *nexus_string_push(string_t *dest, const char c)
{
    NULL_CHECK_PTR(dest);
    IS_OWNED(dest);
    if (dest->used == dest->length)
        return NULL;
    dest->data[dest->used++] = c;
    return dest;
}


const char *nexus_string_pbrk(const string_t *str, const string_t *accept)
{
    NULL_CHECK_PTR(str);
    NULL_CHECK_PTR(accept);
    for (int i = 0; i < str->length; i++)
    {
        const int index = nexus_string_chr(str, accept->data[i]);
        if (index >= 0)
            return &accept->data[i];
    }
    return NULL;
}

void *nexus_memset(void *_, const int c, const uint64_t n)
{
    NULL_CHECK_PTR(_);
    char *dest = _;
    for (int i = 0; i < n; ++i)
    {
        dest[i] = c;
    }
    return dest;
}

void *nexus_memcpy(void *_, const void *_2, const size_t n)
{
    NULL_CHECK_PTR(_);
    NULL_CHECK_PTR(_2);
    char *dest = _;
    const char *src = _2;

    for (int i = 0; i < n; ++i)
    {
        dest[i] = src[i];
    }
    return dest;
}


size_t internal_strlen(const char *str)
{
    NULL_CHECK(str, 0);
    const char *ptr = str;
    size_t len = 0;
    while (*ptr)
    {
        len++;
        ++ptr;
    }
    return len;
}

string_t *nexus_string_str(const string_t *str, const string_t *token)
{
    NULL_CHECK_PTR(str);
    NULL_CHECK_PTR(token);

    if (token->length > str->length)
        return NULL;
    int index = 0;
    int firstChar = 0;
    for (int i = 0; i < str->length; ++i)
    {
        if (str->data[i] == token->data[index])
        {
            if (index == 0)
                firstChar = i;
            index++;
        }
        else if (index > 0 && token->length != index)
        {
            index = 0;
            firstChar = 0;
        }
    }
    if (index == token->length)
    {
        string_t *substr = nexus_string_view(str, firstChar, token->length);
        return substr;
    }
    return NULL;
}

void nexus_string_rev(const string_t *str)
{
    NULL_CHECK_VOID(str);
    if (!str->owned)
        return;
    for (int i = 0, j = str->length - 1; i < str->length; ++i, j--)
    {
        char tmp = str->data[i];
        str->data[i] = str->data[j];
        str->data[j] = tmp;
    }

}

int nexus_string_rchr(const string_t *str, const char c)
{
    NULL_CHECK(str, -1);
    nexus_string_rev(str);
    int index = nexus_string_chr(str, c);
    nexus_string_rev(str);
    return index;
}

int nexus_string_chr(const string_t *str, const char c)
{
    NULL_CHECK(str, -1);
    for (int i = 0; i < str->length; ++i)
    {
        if (str->data[i] == c)
            return i;
    }
    return -1;
}

int nexus_string_ncmp(const string_t *a, const string_t *b, const size_t n)
{
    NULL_CHECK(a, -1);
    NULL_CHECK(b, -1);
    if (n > a->length || n > b->length)
        return -1;
    for (int i = 0; i < n; ++i)
    {
        if (a->data[i] != b->data[i])
            return -1;
    }
    return 0;
}

int nexus_string_cmp(const string_t *a, const string_t *b)
{
    NULL_CHECK(a, -1);
    NULL_CHECK(b, -1);
    if (a->length != b->length)
        return -1;
    for (int i = 0; i < a->length; ++i)
    {
        if (a->data[i] != b->data[i])
            return -1;
    }
    return 0;
}

string_t *nexus_string_cpy(string_t *dest, const string_t *src)
{
    NULL_CHECK_PTR(src);
    NULL_CHECK_PTR(dest);
    IS_OWNED(dest);
    if (src->length > dest->length)
        return NULL;
    for (int i = 0; i < src->length; i++)
    {
        dest->data[i] = src->data[i];
    }
    dest->used = src->length;
    return dest;
}

string_t *nexus_string_ncpy(string_t *dest, const string_t *src, size_t n)
{
    NULL_CHECK_PTR(src);
    NULL_CHECK_PTR(dest);
    IS_OWNED(dest);
    if (n > dest->length || n > src->length)
        return NULL;
    for (int i = 0; i < n; ++i)
    {
        dest->data[i] = src->data[i];
    }
    dest->used = n;
    return dest;
}

string_t *nexus_string_from_cstr(const char *str)
{
    NULL_CHECK_PTR(str);
    int len = internal_strlen(str);
    char *newValue = globalState.allocate(len, sizeof(char), __alignof(char));
    nexus_memcpy(newValue, str, len);
    string_t *newStr = globalState.allocate(1, sizeof(string_t), __alignof(string_t));
    *newStr = (string_t){len, len, newValue,true};
    return newStr;
}

string_t *nexus_string_alloc(const uint64_t len)
{
    string_t *newStr = globalState.allocate(1, sizeof(string_t), __alignof(string_t));
    char *data = globalState.allocate(len, sizeof(char), __alignof(char));
    *newStr = (string_t){len, 0, data,true};
    nexus_memset(data, 0, len);
    return newStr;
}

string_t *nexus_string_wrap(const char *str)
{
    NULL_CHECK_PTR(str);
    uint64_t len = internal_strlen(str);
    string_t *s = globalState.allocate(1, sizeof(string_t), __alignof(string_t));
    *s = (string_t){len, len, (char *)str,false};
    return s;
}

string_t *nexus_string_view(const string_t *str, uint64_t pos, uint64_t len)
{
    NULL_CHECK_PTR(str);
    if (pos > str->length || len > str->length)
        return NULL;
    string_t *s = globalState.allocate(1, sizeof(string_t), __alignof(string_t));
    *s = (string_t){len, len, &str->data[pos],false};
    return s;
}

uint64_t nexus_string_len(const string_t *str)
{
    return str->length;
}

uint64_t nexus_string_used(const string_t *str)
{
    return str->used;
}

uint64_t *nexus_internal_string_mut_used(const string_t *str)
{
    return &((string_t *)str)->used;
}

char nexus_internal_parse_c(const char *str)
{
    if (nexus_string_cmp(nexus_string_wrap(str), nexus_string_wrap("amp")) == 0)
    {
        return '&';
    }
    else if (nexus_string_cmp(nexus_string_wrap(str), nexus_string_wrap("apos")) == 0)
    {
        return '\'';

    }
    else if (nexus_string_cmp(nexus_string_wrap(str), nexus_string_wrap("lt")) == 0)
    {
        return '<';
    }
    else if (nexus_string_cmp(nexus_string_wrap(str), nexus_string_wrap("gt")) == 0)
    {
        return '>';

    }
    else if (nexus_string_cmp(nexus_string_wrap(str), nexus_string_wrap("quot")) == 0)
    {

        return '\"';
    }
    assert(false);
    return 0;
}

const char *nexus_string_data(const string_t *str)
{
    return str->data;
}

char *nexus_string_mut_data(const string_t *str)
{
    assert(str->owned);
    IS_OWNED(str);

    return str->data;
}

uint32_t utf8_parse(const char *str, uint32_t *out)
{
    NULL_CHECK(str, -1);
    NULL_CHECK(out, -1);
    // Normal ASCII
    if ((str[0] & 0x80) == 0)
    {
        *out = str[0];
        return 1;
    }
    else if ((str[0] & 0xE0) == 0xC0)
    {
        if (!IS_CONT_BYTE(str[1]))
            return -1;
        *out = (str[0] & 0x1F) << 6 | (str[1] & 0x3F);
        return 2;
    }
    else if ((str[0] & 0xF0) == 0xE0)
    {
        if (!IS_CONT_BYTE(str[1]) || !IS_CONT_BYTE(str[2]))
            return -1;
        *out = (str[0] & 0x0F) << 12 | (str[1] & 0x3F) << 6 | (str[2] & 0x3F);
        return 3;
    }
    else if ((str[0] & 0xF8) == 0xF0)
    {
        if (!IS_CONT_BYTE(str[1]) || !IS_CONT_BYTE(str[2]) || !IS_CONT_BYTE(str[3]))
            return -1;
        *out = (str[0] & 0x07) << 18 | (str[1] & 0x3F) << 12 | (str[2] & 0x3F) << 6 | (str[3] & 0x3F);
        return 4;
    }
    return -1;
}

void nexus_internal_string_init(void *(*allocate)(uint64_t, uint64_t, uint64_t), void (*deallocate)(void *))
{
    globalState.allocate = allocate;
    globalState.deallocate = deallocate;
};

bool is_all_whitespace(const string_t *str)
{
    for (size_t i = 0; i < str->used; i++)
    {
        char c = (str->data)[i];
        if (!isspace((unsigned char)c))
            return false;
    }
    return true;
}
