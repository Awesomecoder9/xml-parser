#include "utils.h"
#include "tree.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_attr(const char *key, const char* value)
{
    printf("\t%s - %s\n", key, value);
}
void print_tree(const void* _)
{
    const node_t* node = _;
    printf("\t<%s>\n", node->name);
}

void array_reset(array_t *a)
{
    a->pos = 0;
    memset(a->data, 0, sizeof(a->data));
}
