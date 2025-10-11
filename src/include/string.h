#pragma once
#include <stdint.h>
#include "../internal_globals.h"

typedef struct i_string_t string_t;

// --- Memory Manipulation Functions ---
/**
 * @brief Fills a block of memory with a specified byte value.
 *
 * The `nexus_memset()` function fills the first `n` bytes of the memory area
 * pointed to by `dest` with the constant byte `c`.
 * This works exactly like the standard C library function `memset`.
 *
 * @param dest Pointer to the block of memory to fill.
 * @param c The value to be set. It is passed as an `int` but is treated as an `unsigned char`.
 * @param n The number of bytes to be set to the value.
 * @return Returns the original value of `dest`.
 */
NEXUS_PARSER_API void *nexus_memset(void *dest, int c, uint64_t n);

/**
 * @brief Copies a block of memory from a source to a destination.
 *
 * The `nexus_memcpy()` function copies `n` bytes from the memory area `src`
 * to the memory area `dest`. The memory areas must not overlap.
 * This works exactly like the standard C library function `memcpy`.
 *
 * @param dest Pointer to the destination array where the content is to be copied.
 * @param src Pointer to the source of data to be copied.
 * @param n The number of bytes to copy.
 * @return Returns the original value of `dest`.
 */
NEXUS_PARSER_API void *nexus_memcpy(void *dest, const void *src, uint64_t n);


// --- Custom String Functions based on `string_t` ---

/**
 * @brief Finds the first occurrence of any character from a set within a string.
 *
 * The `nexus_string_pbrk()` function locates the first occurrence in the string
 * pointed to by `str->data` of any character in the string pointed to by `accept->data`.
 * This works exactly like the standard C library function `strpbrk`.
 *
 * @param str The string to be searched.
 * @param accept The string containing characters to search for.
 * @return A @p char* to the character from `str` that matches one of the characters in `accept`.
 */
NEXUS_PARSER_API const char *nexus_string_pbrk(const string_t *str, const string_t *accept);

/**
 * @brief Finds the first occurrence of a substring.
 *
 * The `nexus_string_str()` function finds the first occurrence of the entire string
 * `token->data` (not including the terminating null character) in the string
 * `str->data`.
 * This works exactly like the standard C library function `strstr`.
 *
 * @param str The string to be searched.
 * @param token The substring to search for.
 * @return A pointer to the beginning of the located substring within `str`'s data,
 * or `NULL` if the substring is not found.
 */
NEXUS_PARSER_API string_t *nexus_string_str(const string_t *str, const string_t *token);

/**
 * @brief Reverses a string in place.
 *
 * The `nexus_string_rev()` function reverses the order of characters in the string
 * pointed to by `str->data`.
 * This works exactly like the non-standard (but common) library function `strrev`.
 *
 * @param str The string structure containing the data to be reversed.
 */
NEXUS_PARSER_API void nexus_string_rev(const string_t *str);

/**
 * @brief Locates the last occurrence of a character in a string.
 *
 * The `nexus_string_rchr()` function searches for the last occurrence of the character `c`
 * in the string pointed to by `str->data`. The terminating null byte is included
 * in the search.
 * This works exactly like the standard C library function `strrchr`.
 *
 * @param str The string to be searched.
 * @param c The character to be located.
 * @return The index of the last occurrence of `c` in `str`'s data, or a negative value (e.g., -1) if the character is not found.
 */
NEXUS_PARSER_API int nexus_string_rchr(const string_t *str, char c);

/**
 * @brief Locates the first occurrence of a character in a string.
 *
 * The `nexus_string_chr()` function searches for the first occurrence of the character `c`
 * in the string pointed to by `str->data`. The terminating null byte is considered
 * part of the string.
 * This works exactly like the standard C library function `strchr`.
 *
 * @param str The string to be searched.
 * @param c The character to be located.
 * @return The index of the first occurrence of `c` in `str`'s data, or a negative value (e.g., -1) if the character is not found.
 */
NEXUS_PARSER_API int nexus_string_chr(const string_t *str, char c);

/**
 * @brief Compares two strings up to a specified number of characters.
 *
 * The `nexus_string_ncmp()` function lexicographically compares the strings
 * pointed to by `a->data` and `b->data`, but compares at most `n` bytes.
 * This works exactly like the standard C library function `strncmp`.
 *
 * @param a The first string structure to be compared.
 * @param b The second string structure to be compared.
 * @param n The maximum number of bytes to compare.
 * @return An integer less than, equal to, or greater than zero if `a` is found,
 * respectively, to be less than, to match, or be greater than `b`.
 */
NEXUS_PARSER_API int nexus_string_ncmp(const string_t *a, const string_t *b, uint64_t n);

/**
 * @brief Compares two strings.
 *
 * The `nexus_string_cmp()` function lexicographically compares the strings
 * pointed to by `a->data` and `b->data`.
 * This works exactly like the standard C library function `strcmp`.
 *
 * @param a The first string structure to be compared.
 * @param b The second string structure to be compared.
 * @return An integer less than, equal to, or greater than zero if `a` is found,
 * respectively, to be less than, to match, or be greater than `b`.
 */
NEXUS_PARSER_API int nexus_string_cmp(const string_t *a, const string_t *b);

/**
 * @brief Copies one string to another.
 *
 * The `nexus_string_cpy()` function copies the string from `src` to `dest`.
 * The destination string must be large enough to hold the source string.
 * This works exactly like the standard C library function `strcpy`.
 *
 * @param dest The destination string structure.
 * @param src The source string structure.
 * @return Returns the original value of `dest`.
 */
NEXUS_PARSER_API string_t *nexus_string_cpy(string_t *dest, const string_t *src);

/**
 * @brief Copies a specified number of characters from one string to another.
 *
 * The `nexus_string_ncpy()` function copies up to `n` characters from the string
 * pointed to by `src` to the string pointed to by `dest`.
 * This works exactly like the standard C library function `strncpy`.
 *
 * @param dest The destination string structure.
 * @param src The source string structure.
 * @param n The maximum number of characters to copy.
 * @return Returns the original value of `dest`.
 */
NEXUS_PARSER_API string_t *nexus_string_ncpy(string_t *dest, const string_t *src, uint64_t n);

/**
 * @brief Appends one string to another.
 *
 * The `nexus_string_cat()` function appends the string from `src` to the end of
 * the string in `dest`. The memory available in `dest` must be sufficient.
 * This works exactly like the standard C library function `strcat`.
 *
 * @param dest The destination string structure where content is appended.
 * @param src The source string structure to append.
 * @return Returns the original value of `dest`.
 */
NEXUS_PARSER_API string_t *nexus_string_cat(string_t *dest, const string_t *src);

/**
 * @brief
 * @param str An owned null-terminated string to populate the buffer with
 * @return a @p string_t* object with data pointing to the str parameter
 */
NEXUS_PARSER_API string_t *nexus_string_from_cstr(const char *str);

// ---- Constructors ----
NEXUS_PARSER_API string_t *nexus_string_alloc(uint64_t len);

/**
 @brief Used to wrap an emtpy buffer into a @struct string_t
 * @note If str is not empty (not in indeterminate state) meaning it has been written to, use nexus_string_from_cstring or nexus_string_view for a non-owned buffer.
 */
NEXUS_PARSER_API string_t *nexus_string_wrap(const char *str);

NEXUS_PARSER_API string_t *nexus_string_view(const string_t *str, uint64_t pos, uint64_t len);


//---- Accessors ----
NEXUS_PARSER_API uint64_t nexus_string_len(const string_t *str);

NEXUS_PARSER_API uint64_t nexus_string_used(const string_t *str);

NEXUS_PARSER_API const char *nexus_string_data(const string_t *str);

NEXUS_PARSER_API char *nexus_string_mut_data(const string_t *str);



