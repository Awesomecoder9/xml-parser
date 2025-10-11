#pragma once
#include "dom.h"
#include "string.h"

typedef enum
{
    // Initial state
    PARSE_START,
    TEXT_TAG,
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

/**
 * @brief Parses a valid XML document and returns the formed document tree
 * @param xmldoc A @struct string_t object pointing to a valid XML document
 * @returns a  @p node_t* pointer to the root node of the parsed tree, NULL pointer if parsing fails
*/
NEXUS_PARSER_API node_t *nexus_xml_parse(const string_t *xmldoc);