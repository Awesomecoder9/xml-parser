#include "include/file.h"
#include "internal_string.h"
#include "include/arena.h"
#include "include/string.h"
#include <stdio.h>
#include <stdlib.h>

extern _Thread_local struct Context ctx;


void nexus_file_stream_read(file_info_t *_, string_t **buffer)
{
    NULL_CHECK_VOID(_);
    NULL_CHECK_VOID(buffer);
    nexus_temp_t scratch = ctx.get_scratch_free(NULL, 0);
    file_info_t file = *_;
    *buffer = nexus_string_alloc(ctx.arena, file.fileSize + 1);
    char *buf = new(scratch.arena, char, file.fileSize + 1);
    uint64_t bytesRead = fread(buf, sizeof(char), file.fileSize, file.fp);
    assert(bytesRead == file.fileSize);
    buf[file.fileSize] = '\0';
    nexus_string_cpy(*buffer, nexus_string_wrap(scratch.arena, buf));
    temp_end(scratch);
}

file_info_t *
nexus_file_stream_open(const string_t *path, const string_t *modes)
{
    FILE *fp = fopen(nexus_string_data(path), nexus_string_data(modes));
    NULL_CHECK_PTR(fp);
    fseek(fp, 0,SEEK_END);
    uint64_t pos = ftell(fp);
    rewind(fp);
    file_info_t *info = new(ctx.arena, file_info_t, 1);
    *info = (file_info_t){fp, pos};
    return info;
}

void nexus_file_stream_close(file_info_t **_)
{
    NULL_CHECK_VOID(_);
    file_info_t file = **_;
    NULL_CHECK_VOID(file.fp);
    fclose(file.fp);
    **_ = (file_info_t){NULL, 0};
}