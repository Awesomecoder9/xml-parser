#pragma once
#include <stddef.h>
#include <stdint.h>

#define BUFFER_SIZE 256
typedef struct array
{
    char data[256];
    uint16_t pos;
} array_t;


void print_attr(const char *, const char*);
void print_tree(const void*);
void array_reset(array_t *);
