#include "utils.h"

void print_attr(const char *key, const char* value)
{
    printf("\t%s - %s\n", key, value);
}

void array_reset(array_t *a)
{
    a->pos = 0;
    memset(a->data, 0, sizeof(a->data));
}