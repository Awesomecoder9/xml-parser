#pragma once
#include <stdlib.h>
#include <string.h>
#include "utils.h"

typedef struct attr_t
{
    char *name;
    char *value;
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
    char *name;
    attr_t *attrs;
    char *content;
    struct node_t *parent;
    struct node_t *first_child;
    struct node_t *next_sibling;
} node_t;



node_t *node_create(char *data, node_t *parent);
node_t *node_add_child(node_t *);
void node_add_attribute(node_t *, attr_t*);
node_t *node_traverse(node_t *, void (*callback)(void *));

//Insert a node at the beginning of the linked list
void node_attr_push_front( attr_t**);
void node_attr_foreach(attr_t *, void (*callback)(const char *, const char *));
attr_t* attr_create(const char*, const char*);