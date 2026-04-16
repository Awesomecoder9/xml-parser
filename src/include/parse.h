#pragma once
#include "dom.h"
#include "string.h"

typedef enum {
    // Initial state
    PARSE_START,
    XML_COMMENT,
    TAG_TEXT,
    // When encountering '<' IN THE OPENING TAG
    TAG_IN,
    // When encountering any character except ' ' or '>' IN THE OPENING TAG
    TAG_NAME,
    // When encountering a space then a "key"= IN THE OPENING TAG
    TAG_ATTR_NAME,
    // When encountering ="value" IN THE OPENING TAG
    TAG_ATTR_VALUE,
    // When encountering "</"
    TAG_OUT
} parsing_state_t;

typedef enum {
    NEXUS_OK = 0,

    // Structural / syntax
    NEXUS_ERR_UNEXPECTED_EOF,
    NEXUS_ERR_UNCLOSED_TAG,
    NEXUS_ERR_UNEXPECTED_CLOSE_TAG,
    NEXUS_ERR_TAG_MISMATCH,
    NEXUS_ERR_INVALID_ATTRIBUTE,
    NEXUS_ERR_MISSING_GT,
    NEXUS_ERR_DUPLICATE_ROOT,
    NEXUS_ERR_INVALID_COMMENT,

    // Encoding / char
    NEXUS_ERR_INVALID_UTF8,
    NEXUS_ERR_ILLEGAL_CHAR,

    // Memory / internal
    NEXUS_ERR_OUT_OF_MEMORY,
    NEXUS_ERR_DEPTH_OVERFLOW,
    NEXUS_ERR_INTERNAL_STATE,

    NEXUS_ERR_UNKNOWN
} nexus_error_code_t;

static const char *nexus_error_messages[] = {
    [NEXUS_OK] = "No error",
    [NEXUS_ERR_UNEXPECTED_EOF] = "Unexpected end of file",
    [NEXUS_ERR_UNCLOSED_TAG] = "Unclosed tag",
    [NEXUS_ERR_UNEXPECTED_CLOSE_TAG] = "Unexpected closing tag",
    [NEXUS_ERR_TAG_MISMATCH] = "Mismatched tag",
    [NEXUS_ERR_INVALID_ATTRIBUTE] = "Invalid attribute syntax",
    [NEXUS_ERR_MISSING_GT] = "Missing '>' at end of tag",
    [NEXUS_ERR_DUPLICATE_ROOT] = "Duplicate root element",
    [NEXUS_ERR_INVALID_COMMENT] = "Invalid comment syntax",
    [NEXUS_ERR_INVALID_UTF8] = "Invalid UTF-8 sequence",
    [NEXUS_ERR_ILLEGAL_CHAR] = "Illegal character",
    [NEXUS_ERR_OUT_OF_MEMORY] = "Out of memory",
    [NEXUS_ERR_DEPTH_OVERFLOW] = "Element nesting too deep",
    [NEXUS_ERR_INTERNAL_STATE] = "Internal parser state corrupted",
    [NEXUS_ERR_UNKNOWN] = "Unknown error",
};


typedef struct {
    nexus_error_code_t code;
    const string_t *const message;
} error_t;

NEXUS_PARSER_API const string_t *nexus_get_error(nexus_error_code_t code);

/**
 * @brief Parses a valid XML document and returns the formed document tree.
 * * This function takes a string containing a well-formed XML document,
 * parses it, and constructs an in-memory document object model (DOM) tree.
 * The root of this tree is returned via the out_root pointer.
 *
 * @param xmldoc   A constant pointer to a \p string_t object holding the valid XML document content.
 * @param out_root A pointer-to-a-pointer (\p node_t**) that will be set to point to
 * the root node of the newly parsed document tree upon success.
 * If parsing fails, the pointer it points to will be set to \c NULL
 * @returns        An \p error_t object. It contains a return code indicating success
 * or the specific error
 * along with any associated data.
 * @note           The caller is typically responsible for freeing the memory
 * allocated for the resulting document tree (the \p node_t structure).
 * @see            \p node_t, \p string_t, \p error_t
 */
NEXUS_PARSER_API error_t nexus_dom_xml_parse(const string_t *xmldoc, node_t **out_root);
