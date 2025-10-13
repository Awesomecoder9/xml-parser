#include "internal_utils.h"

#include "internal_dom.h"
#include "internal_string.h"
#include "include/dom.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_attr(const string_t *key, const string_t *value)
{
    printf("\tAttribute: \"%s\"=\"%s\"\n", nexus_string_data(key), nexus_string_data(value));
}

void print_tree(const node_t *node)
{
    if (node->type == ELEM_TYPE)
        printf("Element(%s)\n", nexus_string_data(node->name));
    else
        printf("\tText(\"%s\")\n", nexus_string_data(node->content));
    nexus_internal_node_attr_foreach(node->attrs, print_attr);
}

void nexus_internal_string_reset(string_t *a)
{
    *nexus_internal_string_mut_used(a) = 0;
    nexus_memset(nexus_string_mut_data(a), 0, nexus_string_len(a));
}
