#pragma once
#include "../internal_globals.h"
#include "string.h"

typedef struct attr_t
{
    string_t *name;
    string_t *value;
    struct attr_t *next;
} attr_t;

typedef enum
{
    TEXT_TYPE,
    ELEM_TYPE
} node_type;

typedef struct node_t
{
    node_type type;

    union
    {
        string_t *name;
        string_t *content;
    };

    attr_t *attrs;
    struct node_t *parent;
    struct node_t *first_child;
    struct node_t *next_sibling;
} node_t;


NEXUS_PARSER_API node_t *nexus_node_create(string_t *name, node_t *parent);

NEXUS_PARSER_API node_t *nexus_node_add_child(node_t *);

NEXUS_PARSER_API void nexus_node_add_attribute(node_t *, attr_t *);

NEXUS_PARSER_API node_t *nexus_node_traverse(node_t *, void (*callback)(const void *));
