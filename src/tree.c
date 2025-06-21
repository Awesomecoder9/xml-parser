#include "tree.h"

node_t *node_create(char *data, node_t *parent)
{
    node_t *node = (node_t *)malloc(sizeof(node_t));
    node->data = data;
    node->parent = parent;
    return node;
}

void node_set_data(node_t *node, char *data)
{
    size_t inputLen = strlen(data);
    node->data = (char *)malloc(inputLen + 1);
    node->data[inputLen] = '\0';
    strncpy(node->data, data, inputLen);
}

node_t *node_add_child(node_t *node)
{
    if (node->first_child == NULL)
    {
        node->first_child = node_create(NULL, node);
        return node->first_child;
    }
    else
    {
        node_t *currentSibling = node->first_child;
        while (node->next_sibling != NULL)
        {
            currentSibling = currentSibling->next_sibling;
        }
        node_t *newChild = node_create(NULL, node);
        currentSibling->next_sibling = newChild;
        return newChild;
    }
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
