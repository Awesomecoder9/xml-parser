#pragma once
#include <stdlib.h>
#include <string.h>

typedef struct node_t
{
    char *data;
    struct node_t *parent;
    struct node_t *first_child;
    struct node_t *next_sibling;
} node_t;

node_t *node_create(char *data, node_t *parent);
void node_set_data(node_t *node, char *data);
node_t *node_add_child(node_t *);
node_t *node_traverse(node_t *, void (*callback)(void *));
