#include "include/parse.h"
#include "include/arena.h"
#include "include/dom.h"
#include "include/string.h"
#include "internal_dom.h"
#include "internal_string.h"
#include "internal_utils.h"

#include "include/file.h"

#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// TODO: Final version should be a dynamic library
// TODO: Implement error handling
// TODO: Implement permanent and temporary arenas for more efficient allocation

/* XML Parsing rules
Rules:
    One root element only (<data>...</data>)
    Every tag must close (</likeThis>)
    Attributes in quotes (key="value")
    Tags must nest properly (<a><b></b></a>, not <a><b></a></b>)
    Case-sensitive! <Title> ≠ <title>
*/

_Thread_local struct Context ctx;

nexus_temp_t nexus_internal_get_scratch_free(nexus_arena_t **, uint64_t);

__attribute__((constructor)) void ctor()
{
    ctx.get_scratch_free = nexus_internal_get_scratch_free;
    ctx.arena = malloc(sizeof(nexus_arena_t));
    ctx.scratchArenas[0] = malloc(sizeof(nexus_arena_t));
    ctx.scratchArenas[1] = malloc(sizeof(nexus_arena_t));
    arena_init(ctx.arena, KB(30));
    arena_init(ctx.scratchArenas[0], KB(10));
    arena_init(ctx.scratchArenas[1], KB(10));
}

__attribute__((destructor)) void dtor()
{
    arena_free(ctx.arena);
    arena_free(ctx.scratchArenas[0]);
    arena_free(ctx.scratchArenas[1]);
}

static char *nexus_internal_parse_c(const char *str)
{
    nexus_temp_t temp = nexus_internal_get_scratch_free(NULL, 0);
    char *chr = malloc(1);
#define nexus_string_wrap(str) nexus_string_wrap(temp.arena, str)
    if (nexus_string_cmp(nexus_string_wrap(str), nexus_string_wrap("amp")) == 0)
    {
        *chr = '&';
    }
    else if (nexus_string_cmp(nexus_string_wrap(str), nexus_string_wrap("apos"))
             == 0)
    {
        *chr = '\'';
    }
    else if (nexus_string_cmp(nexus_string_wrap(str), nexus_string_wrap("lt"))
             == 0)
    {
        *chr = '<';
    }
    else if (nexus_string_cmp(nexus_string_wrap(str), nexus_string_wrap("gt"))
             == 0)
    {
        *chr = '>';
    }
    else if (nexus_string_cmp(nexus_string_wrap(str), nexus_string_wrap("quot"))
             == 0)
    {
        *chr = '\"';
    }
    else
        assert(false);

    temp_end(temp);
    return chr;
#undef nexus_string_wrap
}

nexus_temp_t nexus_internal_get_scratch_free(nexus_arena_t **conflictingArenas,
                                             uint64_t conflictingArenasSize)
{
    for (int i = 0; i < GET_COUNT(ctx.scratchArenas); ++i)
    {
        bool isConflicting = false;
        for (int j = 0; j < conflictingArenasSize; ++j)
        {
            if (ctx.scratchArenas[i] == conflictingArenas[j])
            {
                isConflicting = true;
                break;
            }
        }
        if (!isConflicting)
            return temp_begin(ctx.scratchArenas[i]);
    }
    return (nexus_temp_t){0, NULL};
}

static bool nexus_internal_parse_helper_inc(
    const char **ptr, uint32_t *codepoint, uint32_t *index,
    int32_t nbytes)
{
    nbytes = nexus_internal_utf8_parse(*ptr, codepoint);
    if (nbytes < 1)
    {
        return false;
    }
    *ptr += nbytes;
    *index += nbytes;
    return true;
}

static inline bool nexus_internal_parse_helper_dec(
    const char **ptr, uint32_t *codepoint, uint32_t *index,
    int32_t *nbytes)
{
    do
    {
        (*ptr)--;
        (*index)--;
        *nbytes = nexus_internal_utf8_parse(*ptr, codepoint);
    } while (*nbytes == -1);
    return true;
}

static char *nexus_internal_parse_helper_special_chr(const char **ptr)
{
    char buf[5] = {0};
    int counter = -1;

    while (*(++*ptr) != ';')
    {
        if (counter > 4)
        {
            ptr -= counter;
        }
        counter++;
        buf[counter] = **ptr;
    }
    return nexus_internal_parse_c(buf);
}

inline const string_t *nexus_get_error(nexus_error_code_t code)
{
#define nexus_string_wrap(str) nexus_string_wrap(ctx.arena, str)
    if (code < 0 || code >= NEXUS_ERR_UNKNOWN)
        return nexus_string_wrap("Unknown error");
    return nexus_string_wrap(nexus_error_messages[code]);
}

error_t nexus_dom_xml_parse(const string_t *xmldoc, node_t **out_root)
{
    nexus_temp_t temp = nexus_internal_get_scratch_free(NULL, 0);
    parsing_state_t parsingState = PARSE_START;
    node_t *root = NULL;
    node_t *traverser = root;
    const char *ptr = nexus_string_data(xmldoc);
    uint32_t index = 0;
    string_t *buffer = nexus_string_alloc(temp.arena, BUFFER_SIZE);
    string_t *keyBuffer = nexus_string_alloc(temp.arena, BUFFER_SIZE);
    string_t *valueBuffer = nexus_string_alloc(temp.arena, BUFFER_SIZE);
    attr_t *attrBuffer = NULL;

    // Parsing loop
    while (index < nexus_string_used(xmldoc))
    {
        uint32_t codepoint;
        int32_t nbytes = nexus_internal_utf8_parse(ptr, &codepoint);
        if (nbytes < 1)
        {
            return (error_t){NEXUS_ERR_INVALID_UTF8};
        }
        switch (parsingState)
        {
        case PARSE_START:
            if (*ptr == '<' && *(ptr + 1) == '!' && *(ptr + 2) == '-' && *(ptr + 3) ==
                '-')
            {
                parsingState = XML_COMMENT;
            }
            else if (*ptr == '<' && *(ptr + 1) == '/')
            {
                if (!nexus_internal_parse_helper_inc(&ptr, &codepoint, &index,
                                                     nbytes))
                {
                    return (error_t){NEXUS_ERR_ILLEGAL_CHAR,
                                     nexus_string_wrap("Invalid UTF-8 character")};
                }
                parsingState = TAG_OUT;
            }
            else if (*ptr == '<' && *(ptr + 1) != '/')
            {
                parsingState = TAG_IN;
            }
            else if (*ptr != '<')
            {
                nexus_internal_parse_helper_dec(&ptr, &codepoint, &index, &nbytes);
                parsingState = TAG_TEXT;
            }
            break;
        case XML_COMMENT:
            if (*ptr == '-' && *(ptr + 1) == '-' && *(ptr + 2) == '>')
            {
                parsingState = PARSE_START;
                ptr += 2;
            }
            break;
        case TAG_TEXT:
            if (*ptr == '<' && *(ptr + 1) == '!' && *(ptr + 2) == '-' && *(ptr + 3) ==
                '-')
            {
                parsingState = XML_COMMENT;
            }
            else if (*ptr == '<')
            {
                nexus_internal_parse_helper_dec(&ptr, &codepoint, &index, &nbytes);
                if (!is_all_whitespace(buffer))
                {
                    traverser = nexus_node_add_child(traverser);
                    nexus_string_cpy(traverser->content, buffer);
                }
                nexus_string_reset(buffer);
                parsingState = PARSE_START;
            }
            else
            {
                if (*ptr == '&')
                {
                    nexus_string_putc(
                        buffer, nexus_internal_parse_helper_special_chr(&ptr));
                    goto end;
                }
                nexus_string_putc(buffer, ptr);
            }
            break;
        case TAG_IN:
            if (*ptr == '>' || (*ptr == '?' && *(ptr + 1) == '>') || (
                    *ptr == '/' && *(ptr + 1) == '>'))
            {
                if (!traverser)
                {
                    root = nexus_node_create(NULL, NULL);
                    traverser = root;
                }
                // Add a child node to the current node, set its data to the parsed opening tag and reset buffers
                if (*ptr != '?')
                    traverser = nexus_node_add_child(traverser);
                else
                    ptr++;
                nexus_string_cpy(traverser->name, buffer);
                nexus_node_add_attribute(traverser, attrBuffer);
                traverser->type = ELEM_TYPE;
                printf("Opened <%.*s>\n", NEXUS_STR_FMT(traverser->name));
                if (*ptr == '/' && *(ptr + 1) == '>')
                {
                    parsingState = TAG_OUT;
                    goto end;
                }

                nexus_string_reset(buffer);
                nexus_string_reset(keyBuffer);
                nexus_string_reset(valueBuffer);

                attrBuffer = NULL;
                parsingState = PARSE_START;
            }
            else if (*ptr == '?')
            {
                goto end;
            }
            else if (*ptr == '<' || (*ptr == '>' && (*ptr + 1) == '/'))
            {
                return (error_t){NEXUS_ERR_MISSING_GT};
            }
            // else if (*ptr == ' ') {
            //     if (isalpha(*(ptr + 1)) || *(ptr + 1) == '_')
            //         parsingState = TAG_ATTR_NAME;
            //     else
            //         return (error_t){NEXUS_ERR_MISSING_GT};
            // }
            else
            {
                nexus_internal_parse_helper_dec(&ptr, &codepoint, &index, &nbytes);
                parsingState = TAG_NAME;
            }
            break;
        case TAG_NAME:
            /*
             *If encounters space, handles attribute key. Else, fills the buffer with the tag name or
            goes back to inside the tag(<>)*/
            if (*ptr == ' ')
            {
                if (isalpha(*(ptr + 1)) || *(ptr + 1) == '_')
                    parsingState = TAG_ATTR_NAME;
                else if (*(ptr + 1) == '/' && *(ptr + 2) == '>')
                    parsingState = TAG_IN;
                else
                    return (error_t){NEXUS_ERR_MISSING_GT};
            }
            else if (*ptr == '>' || *ptr == '/')
            {
                nexus_internal_parse_helper_dec(&ptr, &codepoint, &index, &nbytes);
                parsingState = TAG_IN;
            }
            else if (isspace(*ptr))
            {
                goto end;
            }
            else
            {
                nexus_string_putc(buffer, ptr);
            }
            break;

        case TAG_ATTR_NAME:
            if (*ptr != '=')
            {
                if (*ptr == '&')
                {
                    nexus_string_putc(keyBuffer,
                                      nexus_internal_parse_helper_special_chr(&ptr));
                    goto end;
                }
                nexus_string_putc(keyBuffer, ptr);
            }
            else
            {
                // check if attribute value starts with a double quotation mark

                if (*(ptr + 1) == '"')
                {
                    if (!nexus_internal_parse_helper_inc(&ptr, &codepoint, &index,
                                                         nbytes))
                    {
                        return (error_t){NEXUS_ERR_ILLEGAL_CHAR};
                    }
                    parsingState = TAG_ATTR_VALUE;
                }
                else
                {
                    return (error_t){NEXUS_ERR_INVALID_ATTRIBUTE};
                }
            }
            break;
        case TAG_ATTR_VALUE:
            /* If done filling buffer with the attr value,
             * creates a new attr_t and appends it to the linked list
             * which will be finally appended to the new child node
             * at the tag close(</>)
             */
            if (*ptr == '"')
            {
                if (!attrBuffer)
                {
                    attrBuffer = nexus_internal_attr_create(keyBuffer, valueBuffer);
                }
                else
                {
                    attrBuffer->next = nexus_internal_attr_create(keyBuffer, valueBuffer);
                }
                nexus_string_reset(keyBuffer);
                nexus_string_reset(valueBuffer);
                parsingState = TAG_IN;
            }
            else if (*ptr == '>' || *ptr == '<' || *ptr == ' ')
            {
                return (error_t){NEXUS_ERR_INVALID_ATTRIBUTE};
            }
            else
            {
                if (*ptr == '&')
                {
                    nexus_string_putc(valueBuffer,
                                      nexus_internal_parse_helper_special_chr(&ptr));
                    goto end;
                }
                nexus_string_putc(valueBuffer, ptr);
            }
            break;
        case TAG_OUT:
            if (*ptr != '>')
            {
                nexus_string_putc(buffer, ptr);
            }
            else
            {
                if (traverser->type == TEXT_TYPE && traverser->parent)
                    traverser = traverser->parent;
                printf("Comparing '%.*s' and '%.*s'\n", NEXUS_STR_FMT(traverser->name),
                       NEXUS_STR_FMT(buffer));
                if (nexus_string_cmp(traverser->name, buffer) == 0)
                {
                    printf("Closing <%s>\n", nexus_string_data(traverser->name));
                    traverser = traverser->parent;
                    nexus_string_reset(buffer);
                    nexus_string_reset(keyBuffer);
                    nexus_string_reset(valueBuffer);
                    attrBuffer = NULL;
                    parsingState = PARSE_START;
                }
                else
                {
                    printf("Tag <%.*s> opened but not closed\n",
                           NEXUS_STR_FMT(traverser->name));
                    return (error_t){NEXUS_ERR_TAG_MISMATCH};
                }
            }
            break;
        }
    end:
        ptr += nbytes;
        index += nbytes;
    }
    printf("Parsing completed \n");
    if (traverser != root)
    {
        return (error_t){NEXUS_ERR_UNCLOSED_TAG};
    }
    *out_root = root;
    temp_end(temp);
    return (error_t){NEXUS_OK, NULL};
}

int main(int argc, const char **argv)
{
    file_info_t *info =
        nexus_file_stream_open(
            nexus_string_wrap("/home/plank/Desktop/xml-parser/test.xml"),
            nexus_string_wrap("rb"));

    string_t *str = NULL;
    nexus_file_stream_read(info, &str);
    node_t *rootNode = NULL;
    error_t error = nexus_dom_xml_parse(str, &rootNode);
    if (error.code != NEXUS_OK)
    {
        fprintf(stderr, "Error: %.*s\n",
                NEXUS_STR_FMT(nexus_get_error(error.code)));
        return -1;
    }
    nexus_node_traverse(rootNode, print_tree);
    nexus_file_stream_close(&info);
    return 0;
}
