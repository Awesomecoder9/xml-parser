#include "include/file.h"
#include "include/string.h"
#include <stdio.h>


void nexus_file_stream_read(file_info_t file, string_t *buffer)
{
    if (!file.fp)
        return;
    fread(nexus_string_mut_data(buffer), sizeof(char), nexus_string_len(buffer), file.fp);
}

file_info_t nexus_file_stream_open(const string_t *path, const string_t *modes)
{
    FILE *fp = fopen(nexus_string_data(path), nexus_string_data(modes));
    if (!fp)
        return (file_info_t){};
    fpos_t pos;
    fseek(fp, 0,SEEK_END);
    fgetpos(fp, &pos);
    return (file_info_t){fp, pos.__pos};
}
