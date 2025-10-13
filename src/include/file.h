#pragma once
#include "string.h"

#include <stdio.h>

typedef struct
{
    FILE *fp;
    uint64_t fileSize;
} file_info_t;

NEXUS_PARSER_API void nexus_file_stream_read(file_info_t file, string_t **buffer);

NEXUS_PARSER_API file_info_t nexus_file_stream_open(const string_t *path, const string_t *modes);