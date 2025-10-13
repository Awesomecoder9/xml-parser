#include "include/file.h"

#include "internal_string.h"
#include "include/arena.h"
#include "include/string.h"
#include <stdio.h>

extern arena_t arena;

void nexus_file_stream_read(file_info_t file, string_t **buffer)
{
    if (!file.fp)
        return;
    *buffer = nexus_string_alloc(file.fileSize);
    char *buf = arena_alloc_array(&arena, sizeof(char), file.fileSize, __alignof(char));
    uint64_t bytesRead = fread(buf, sizeof(char), file.fileSize, file.fp);
    buf[file.fileSize] = '\0';
    assert(bytesRead == file.fileSize);
    nexus_string_cpy(*buffer, nexus_string_wrap(buf));
}

file_info_t nexus_file_stream_open(const string_t *path, const string_t *modes)
{

    FILE *fp = fopen(nexus_string_data(path), nexus_string_data(modes));
    if (!fp)
        return (file_info_t){};
    fseek(fp, 0,SEEK_END);
    uint64_t pos = ftell(fp);
    rewind(fp);
    return (file_info_t){fp, pos};
}
