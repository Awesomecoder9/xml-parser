#include <stdio.h>
#include "include/dom.h"
#include "internal_dom.h"
#include <stdlib.h>
#include <string.h>
#include "include/arena.h"

arena_t arena;

node_t *nexus_node_create(string_t *name, node_t *parent)
{
    node_t *node = arena_alloc(&arena, sizeof(node_t), __alignof(node_t));
    if (!name)
    {
        node->name = nexus_string_alloc(BUFFER_SIZE);
    }
    else
        nexus_string_cpy(node->name, name);
    node->parent = parent;
    return node;
}

node_t *nexus_node_add_child(node_t *node)
{
    node_t *firstChild = node->first_child;
    if (firstChild == NULL)
    {
        firstChild = nexus_node_create(NULL, node);
        node->first_child = firstChild;
        return firstChild;
    }
    node_t *currentSibling = firstChild;
    while (currentSibling->next_sibling != NULL)
    {
        currentSibling = currentSibling->next_sibling;
    }
    node_t *newChild = nexus_node_create(NULL, node);
    currentSibling->next_sibling = newChild;

    return newChild;
}

void nexus_node_add_attribute(node_t *node, attr_t *attrs)
{
    if (!node || !attrs)
        return;
    if (!node->attrs)
    {
        node->attrs = attrs;
        return;
    }

    const attr_t *currentAttr = node->attrs;
    while (currentAttr->next)
    {
        currentAttr = currentAttr->next;
    }
}

void nexus_internal_node_attr_push_front(attr_t **head)
{
    if (!head)
    {
        return;
    }
    attr_t *newNode = nexus_internal_attr_create(NULL, NULL);
    newNode->next = *head;
    *head = newNode;
}

void nexus_node_traverse(node_t *node, void (*callback)(const node_t *))
{
    if (!node)
        return;

    callback(node);

    // Traverse all children
    for (node_t *child = node->first_child; child; child = child->next_sibling)
        nexus_node_traverse(child, callback);
}


void nexus_internal_node_attr_foreach(attr_t *attrs, void (*callback)(const string_t *, const string_t *))
{
    if (!attrs)
    {
        return;
    }
    const attr_t *currentAttr = attrs;

    while (currentAttr)
    {
        callback(currentAttr->name, currentAttr->value);
        currentAttr = currentAttr->next;
    }
}

attr_t *nexus_internal_attr_create(const string_t *key, const string_t *value)
{
    attr_t *newNode = arena_alloc(&arena, sizeof(attr_t), __alignof(attr_t));
    nexus_memset(newNode, 0, sizeof(attr_t));
    newNode->name = nexus_string_alloc(BUFFER_SIZE);
    nexus_string_cpy(newNode->name, key);
    newNode->value = nexus_string_alloc(BUFFER_SIZE);
    nexus_string_cpy(newNode->value, value);

    return newNode;
}