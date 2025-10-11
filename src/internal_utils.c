#include "internal_utils.h"

#include "internal_string.h"
#include "include/dom.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_attr(const string_t *key, const string_t *value)
{
    printf("\t%s - %s\n", nexus_string_data(key), nexus_string_data(value));
}

void print_tree(const void *_)
{
    const node_t *node = _;
    printf("\t<%s>\n", nexus_string_data(node->name));
}

void nexus_internal_string_reset(string_t *a)
{
    *nexus_string_mut_used(a) = 0;
    nexus_memset(nexus_string_mut_data(a), 0, nexus_string_len(a));
}
