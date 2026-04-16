#include "internal_utils.h"

#include "internal_dom.h"
#include "internal_string.h"
#include "include/dom.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_attr(const string_t *key, const string_t *value)
{
    printf("\tAttribute: \"%.*s\"=\"%.*s\"\n", NEXUS_STR_FMT(key), NEXUS_STR_FMT(value));
}

void print_tree(const node_t *node)
{
    if (node->type == ELEM_TYPE)
        printf("Element(%.*s)\n", NEXUS_STR_FMT(node->name));
    else
        printf("\tText(\"%.*s\")\n", NEXUS_STR_FMT(node->content));
    nexus_internal_node_attr_foreach(node->attrs, print_attr);
}
