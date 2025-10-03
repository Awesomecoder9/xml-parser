#include <stdio.h>
#include "tree.h"
#include "utils.h"


node_t *node_create(char *name, node_t *parent)
{
    node_t *node = (node_t *)malloc(sizeof(node_t));
    node->name = name;
    node->parent = parent;
    return node;
}

node_t *node_add_child( node_t *node)
{
    node_t* firstChild = node->first_child;
    if (firstChild == NULL)
    {
        firstChild = node_create(NULL, node);
        firstChild->name = (char*)malloc(BUFFER_SIZE);
        memset(firstChild->name,0,BUFFER_SIZE);
        firstChild->content = (char*)malloc(BUFFER_SIZE);
        memset(firstChild->content,0,BUFFER_SIZE);
        return firstChild;
    }
    else
    {
        node_t *currentSibling = firstChild;
        while (currentSibling->next_sibling != NULL)
        {
            currentSibling = currentSibling->next_sibling;
        }
        node_t *newChild = node_create(NULL, node);
        currentSibling->next_sibling = newChild;

        newChild->name = (char*)malloc(BUFFER_SIZE);
        memset(newChild->name,0,BUFFER_SIZE);
        newChild->content = (char*)malloc(BUFFER_SIZE);
        memset(newChild->content,0,BUFFER_SIZE);

        return newChild;
    }
}

void node_add_attribute(node_t *node, attr_t* attrs)
{
    if (!node || !attrs) return;
    if (!node->attrs)
    {
        node->attrs = attrs;
        return;
    }

    attr_t *currentAttr = node->attrs;
    while (currentAttr->next)
    {
        currentAttr = currentAttr->next;
    }
}
void node_attr_push_front(attr_t **head)
{
    if (!head)
    {
       return;
    }
    attr_t* newNode = attr_create(NULL, NULL);
    newNode->next = *head;
    *head = newNode;
}

node_t *node_traverse(node_t *tree, void (*callback)(void *))
{
    node_t *currentNode = tree->first_child;
    if (currentNode->first_child == NULL)
    {
        return tree;
    }
    node_traverse(currentNode, callback);
    if (currentNode->next_sibling != NULL)
    {
        currentNode = currentNode->next_sibling;
        node_traverse(currentNode, callback);
    }
    return tree;
}

void node_attr_foreach(attr_t *attrs, void (*callback)(const char *, const char *))
{
    if(!attrs){return;
    }
    const attr_t *currentAttr = attrs;

    while (currentAttr)
    {
        callback(currentAttr->name, currentAttr->value);
        currentAttr = currentAttr->next;
    }
}

attr_t* attr_create(const char *key, const char *value)
{
    attr_t* newNode = malloc(sizeof(attr_t));
    memset(newNode, 0, sizeof(attr_t));
    newNode->name = malloc(BUFFER_SIZE);
    strncpy(newNode->name, key, BUFFER_SIZE );
    newNode->value = malloc(BUFFER_SIZE);
    strncpy(newNode->value, value,BUFFER_SIZE);

    return  newNode;
}