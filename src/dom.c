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
    node->name = name;
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
        firstChild->name = nexus_string_alloc(BUFFER_SIZE);
        nexus_memset(nexus_string_mut_data(firstChild->name), 0,BUFFER_SIZE);
        return firstChild;
    }
    node_t *currentSibling = firstChild;
    while (currentSibling->next_sibling != NULL)
    {
        currentSibling = currentSibling->next_sibling;
    }
    node_t *newChild = nexus_node_create(NULL, node);
    currentSibling->next_sibling = newChild;

    newChild->name = nexus_string_alloc(BUFFER_SIZE);
    nexus_memset(nexus_string_mut_data(newChild->name), 0,BUFFER_SIZE);

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

node_t *nexus_node_traverse(node_t *tree, void (*callback)(const void *))
{
    node_t *currentNode = tree->first_child;
    if (!currentNode)
    {
        if (tree->next_sibling != NULL)
        {
            currentNode = tree->next_sibling;
            callback(currentNode);
            nexus_node_traverse(currentNode, callback);
            return tree;
        }
        return tree;

    }

    if (currentNode->type == TEXT_TYPE)
        return tree;
    callback(currentNode);

    nexus_node_traverse(currentNode, callback);
    return tree;
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