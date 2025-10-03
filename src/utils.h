#pragma once
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#define BUFFER_SIZE 256
typedef struct array
{
    char data[256];
    uint16_t pos;
} array_t;


void print_attr(const char *, const char*);
void array_reset(array_t *);
