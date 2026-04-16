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
struct i_string_t {
    uint64_t length; /**< The total allocated capacity of the internal buffer. */
    uint64_t used; /**< The current number of characters stored in the string. */
    char *data; /**< Pointer to the underlying character array (buffer). */
    bool owned;
};

#define IS_CONT_BYTE(x) ((x & 0xC0) == 0x80)
#define IS_START_BYTE(x) ((x & 0xC0) != 0x80)
#define IS_OWNED(x) if(!x->owned) return NULL;
extern _Thread_local struct Context ctx;
//TODO: MAKE EVERY STRING FUNCTION UTF-8 COMPATIBLE

string_t *nexus_string_cat(string_t *dest, const string_t *src) {
    NULL_CHECK_PTR(dest);
    NULL_CHECK_PTR(src);
    IS_OWNED(dest);
    const uint64_t newUsed = dest->used + src->used;
    if (newUsed > dest->length) {
        char *newData = new(ctx.arena, char, newUsed);
        nexus_memcpy(newData, dest->data, dest->used);
        dest->data = newData;
        dest->length = newUsed;
        dest->used = newUsed - src->used;
    }
    string_t sub = (string_t){
        dest->length - dest->used, 0,
        &dest->data[dest->used]
    };
    nexus_string_cpy(&sub, src);
    return dest;
}

string_t *nexus_string_putc(string_t *dest, const char *c) {
    NULL_CHECK_PTR(dest);
    IS_OWNED(dest);
    if (dest->used == dest->length)
        return NULL;
    uint32_t codepoint;
    int32_t nbytes = nexus_internal_utf8_parse(c, &codepoint);
    if (nbytes < 1)
        return NULL;
    if (dest->used + nbytes > dest->length)
        return NULL;
    nexus_memcpy(&dest->data[dest->used], c, nbytes);
    dest->used += nbytes;
    return dest;
}


const char *nexus_string_pbrk(const string_t *str, const string_t *accept) {
    NULL_CHECK_PTR(str);
    NULL_CHECK_PTR(accept);
    for (int i = 0, j = 0; i < str->used && j < accept->used;) {
        uint32_t codepoint;
        int32_t strBytes = nexus_internal_utf8_parse(&str->data[i], &codepoint);
        int32_t accBytes = nexus_internal_utf8_parse(&accept->data[j], &codepoint);
        const int index = nexus_string_chr(str, &accept->data[j]);
        if (index >= 0)
            return &accept->data[j];
        i += strBytes;
        j += accBytes;
    }
    return NULL;
}

void *nexus_memset(void *_, const int32_t c, const uint64_t n) {
    NULL_CHECK_PTR(_);
    char *dest = _;
    for (int i = 0; i < n; ++i) {
        dest[i] = c;
    }
    return dest;
}

void *nexus_memcpy(void *_, const void *_2, const size_t n) {
    NULL_CHECK_PTR(_);
    NULL_CHECK_PTR(_2);
    char *dest = _;
    const char *src = _2;

    for (int i = 0; i < n; ++i) {
        dest[i] = src[i];
    }
    return dest;
}


uint64_t nexus_internal_strlen(const char *str) {
    NULL_CHECK(str, -1);
    const char *ptr = str;
    uint64_t len = 0;
    while (*ptr) {
        len++;
        ++ptr;
    }
    return len;
}

uint64_t nexus_utf8_strlen(const string_t *str) {
    NULL_CHECK(str, -1);
    uint32_t codepoint;
    uint64_t len = 0;
    for (int i = 0; i < str->used;) {
        int nbytes = nexus_internal_utf8_parse(&str->data[i], &codepoint);
        if (nbytes < 1) {
            return -1;
        }
        i += nbytes;
        len = i;
    }
    return len;
}

string_t *nexus_string_str(const string_t *str, const string_t *token) {
    NULL_CHECK_PTR(str);
    NULL_CHECK_PTR(token);
    if (token->used > str->used)
        return NULL;

    int tokenOffset = 0;
    int tokenStart = 0;

    for (int stringOffset = 0; stringOffset < str->used;) {
        uint32_t strCp, tokenCp;
        int32_t strBytes = nexus_internal_utf8_parse(&str->data[stringOffset], &strCp);
        if (strBytes < 1) {
            return NULL;
        }
        int32_t tokenBytes = nexus_internal_utf8_parse(&token->data[tokenOffset], &tokenCp);
        if (tokenBytes < 1) {
            return NULL;
        }
        if (strCp == tokenCp) {
            if (tokenOffset == 0)
                tokenStart = stringOffset;
            tokenOffset += tokenBytes;
        } else if (tokenOffset > 0 && tokenOffset != token->used) {
            tokenOffset = 0;
            tokenStart = 0;
        }
        stringOffset += strBytes;
    }
    if (tokenOffset == token->used) {
        string_t *substr = nexus_string_view(ctx.arena, str, tokenStart, token->used);
        return substr;
    }
    return NULL;
}

void nexus_string_rev(const string_t *str) {
    NULL_CHECK_VOID(str);
    if (!str->owned)
        return;
    for (uint64_t i = 0, j = str->used - 1; i < str->used; ++i, j--) {
        char tmp = str->data[i];
        str->data[i] = str->data[j];
        str->data[j] = tmp;
    }
}

int nexus_string_rchr(const string_t *str, const char *c) {
    NULL_CHECK(str, -1);
    nexus_string_rev(str);
    int index = nexus_string_chr(str, c);
    nexus_string_rev(str);
    return index;
}

int nexus_string_chr(const string_t *str, const char *c) {
    NULL_CHECK(str, -1);
    for (int i = 0; i < str->used;) {
        uint32_t codepoint1, codepoint2;
        nexus_internal_utf8_parse(c, &codepoint2);
        int32_t nbytes = nexus_internal_utf8_parse(&str->data[i], &codepoint1);
        if (codepoint1 == codepoint2)
            return i;
        i += nbytes;
    }
    return -1;
}

int nexus_string_ncmp(const string_t *a, const string_t *b, const size_t n) {
    NULL_CHECK(a, -1);
    NULL_CHECK(b, -1);
    if (n > a->used || n > b->used)
        return -1;
    for (int i = 0, j = 0; i < n && j < n;) {
        uint32_t aCp, bCp;
        int32_t aBytes = nexus_internal_utf8_parse(&a->data[i], &aCp);
        int32_t bBytes = nexus_internal_utf8_parse(&b->data[j], &bCp);
        if (nexus_internal_utf8_char_cmp(&a->data[i], &b->data[j]))
            return -1;
        i += aBytes;
        j += bBytes;
    }
    return 0;
}

int nexus_string_cmp(const string_t *a, const string_t *b) {
    NULL_CHECK(a, -1);
    NULL_CHECK(b, -1);
    if (a->used != b->used)
        return -1;
    for (int i = 0; i < a->used; ++i) {
        if (a->data[i] != b->data[i])
            return -1;
    }
    return 0;
}

string_t *nexus_string_cpy(string_t *dest, const string_t *src) {
    NULL_CHECK_PTR(src);
    NULL_CHECK_PTR(dest);
    IS_OWNED(dest);
    if (src->used > dest->length)
        return NULL;
    for (int i = 0; i < src->used; i++) {
        dest->data[i] = src->data[i];
    }
    dest->used = src->used;
    return dest;
}

//Hello World
//goodbye✌️ world
string_t *nexus_string_ncpy(string_t *dest, const string_t *src, size_t n) {
    NULL_CHECK_PTR(src);
    NULL_CHECK_PTR(dest);
    IS_OWNED(dest);
    if (n > nexus_utf8_strlen(dest) || n > nexus_utf8_strlen(src))
        return NULL;

    int destUsed = 0;
    for (int i = 0, j = 0; i < n && j < n; destUsed++) {
        uint32_t destCp, srcCp;
        int32_t destBytes = nexus_internal_utf8_parse(&dest->data[i], &destCp);
        int32_t srcBytes = nexus_internal_utf8_parse(&src->data[j], &srcCp);
        nexus_memcpy(&dest->data[i], &src->data[j], srcBytes);
        i += destBytes;
        j += srcBytes;
    }
    dest->used = destUsed;
    return dest;
}

string_t *nexus_string_from_cstr(nexus_arena_t *arena, const char *str) {
    NULL_CHECK_PTR(str);
    const uint64_t len = nexus_internal_strlen(str);
    char *newValue = new(arena, char, len);
    nexus_memcpy(newValue, str, len);
    string_t *newStr = new(ctx.arena, string_t, 1);
    *newStr = (string_t){len, len, newValue,true};
    return newStr;
}

string_t *nexus_string_alloc(nexus_arena_t *arena, const uint64_t len) {
    string_t *newStr = new(arena, string_t, 1);
    char *data = new(arena, char, len);
    *newStr = (string_t){len, 0, data,true};
    nexus_memset(data, 0, len);
    return newStr;
}

string_t *nexus_string_wrap(nexus_arena_t *arena, const char *str) {
    NULL_CHECK_PTR(str);
    uint64_t len = nexus_internal_strlen(str);
    string_t *s = new(arena, string_t, 1);
    *s = (string_t){len, len, (char *) str,false};
    return s;
}

string_t *nexus_string_view(nexus_arena_t *arena, const string_t *str,
                            uint64_t pos, uint64_t len) {
    NULL_CHECK_PTR(str);
    if (pos > str->used || len > str->used || pos + len > str->used)
        return NULL;
    string_t *s = new(arena, string_t, 1);
    *s = (string_t){len, len, (char *) nexus_string_at(str, pos),false};
    return s;
}

uint64_t nexus_string_len(const string_t *str) {
    NULL_CHECK(str, -1);
    return str->length;
}

uint64_t nexus_string_used(const string_t *str) {
    NULL_CHECK(str, -1);
    return str->used;
}

uint64_t *nexus_internal_string_mut_used(const string_t *str) {
    NULL_CHECK_PTR(str);
    return &((string_t *) str)->used;
}

const char *nexus_string_data(const string_t *str) {
    NULL_CHECK_PTR(str);
    return str->data;
}

void nexus_string_reset(string_t *s) {
    NULL_CHECK_VOID(s);
    nexus_memset(s->data, 0, s->length);
    s->used = 0;
}

const char *nexus_string_at(const string_t *s, uint64_t pos) {
    NULL_CHECK_PTR(s);
    if (pos > s->used)
        return NULL;
    for (int32_t i = 0, nbytes = 0; i < s->used && nbytes < s->length; i++) {
        uint32_t codepoint;
        int inc = nexus_internal_utf8_parse(&s->data[nbytes], &codepoint);
        if (inc < 1)
            return NULL;
        nbytes += inc;
        if (i == pos)
            return &s->data[nbytes];
    }
    return NULL;
}

uint32_t nexus_internal_utf8_parse(const char *str, uint32_t *out) {
    NULL_CHECK(str, -1);
    NULL_CHECK(out, -1);
    // Normal ASCII
    if ((str[0] & 0x80) == 0) {
        *out = str[0];
        return 1;
    } else if ((str[0] & 0xE0) == 0xC0) {
        if (!IS_CONT_BYTE(str[1]))
            return -1;
        *out = (str[0] & 0x1F) << 6 | (str[1] & 0x3F);
        return 2;
    } else if ((str[0] & 0xF0) == 0xE0) {
        if (!IS_CONT_BYTE(str[1]) || !IS_CONT_BYTE(str[2]))
            return -1;
        *out = (str[0] & 0x0F) << 12 | (str[1] & 0x3F) << 6 | (str[2] & 0x3F);
        return 3;
    } else if ((str[0] & 0xF8) == 0xF0) {
        if (!IS_CONT_BYTE(str[1]) || !IS_CONT_BYTE(str[2]) || !IS_CONT_BYTE(str[3]))
            return -1;
        *out = (str[0] & 0x07) << 18 | (str[1] & 0x3F) << 12 | (str[2] & 0x3F) << 6
               | (str[3] & 0x3F);
        return 4;
    }
    return -1;
}

bool nexus_internal_utf8_char_cmp(const char *a, const char *b) {
    uint32_t codepoint1, codepoint2;
    int32_t nbytes = 0;
    nbytes = nexus_internal_utf8_parse(a, &codepoint1);
    if (nbytes < 1)
        return false;
    nbytes = nexus_internal_utf8_parse(b, &codepoint2);
    if (nbytes < 1)
        return false;
    return (codepoint1 == codepoint2);
}

bool is_all_whitespace(const string_t *str) {
    NULL_CHECK(str, -1);
    for (size_t i = 0; i < str->used; i++) {
        char c = str->data[i];
        if (!isspace((unsigned char)c))
            return false;
    }
    return true;
}
