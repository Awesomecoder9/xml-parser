#pragma once
#include "include/dom.h"

#include <stdint.h>
#include "include/string.h"

void print_attr(const string_t *, const string_t *);

void print_tree(const node_t *);

void nexus_internal_string_reset(string_t *);
