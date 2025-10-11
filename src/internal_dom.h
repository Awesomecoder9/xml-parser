#pragma once
#include "include/dom.h"

//Insert a node at the beginning of the linked list
void nexus_internal_node_attr_push_front(attr_t **);

void nexus_internal_node_attr_foreach(attr_t *, void (*callback)(const string_t *, const string_t *));

attr_t *nexus_internal_attr_create(const string_t *, const string_t *);