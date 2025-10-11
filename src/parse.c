#include "internal_string.h"
#include "internal_dom.h"
#include "include/string.h"
#include "include/dom.h"
#include "internal_utils.h"
#include "include/arena.h"
#include "include/parse.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

//TODO: Final version should be a dynamic library
//TODO: Implement error handling

/* XML Parsing rules
Rules:
    One root element only (<data>...</data>)
    Every tag must close (</likeThis>)
    Attributes in quotes (key="value")
    Tags must nest properly (<a><b></b></a>, not <a><b></a></b>)
    Case-sensitive! <Title> ≠ <title>
*/

extern arena_t arena;

void *allocator(uint64_t numOfItems, uint64_t elemSize, uint64_t align)
{
    return arena_alloc_array(&arena, elemSize, numOfItems, align);
}

void deallocator(void *ptr)
{
    return;
}

__attribute__((constructor))
void ctor()
{
    arena_init(&arena, KB(10));
    nexus_internal_string_init(allocator, deallocator);
}

__attribute__((destructor))
void dtor()
{
    arena_free(&arena);
}


node_t *nexus_xml_parse(const string_t *xmldoc)
{
    parsing_state_t parsingState = PARSE_START;
    node_t *root = nexus_node_create(NULL, NULL);
    node_t *traverser = root;
    const register char *ptr = nexus_string_data(xmldoc);
    int index = 0;
    string_t *buffer = nexus_string_alloc(BUFFER_SIZE);
    string_t *keyBuffer = nexus_string_alloc(BUFFER_SIZE);
    string_t *valueBuffer = nexus_string_alloc(BUFFER_SIZE);
    attr_t *attrBuffer = NULL;

    // Parsing loop
    while (index <= nexus_string_len(xmldoc))
    {
        uint32_t codepoint;
        int32_t nbytes = utf8_parse(ptr, &codepoint);

        switch (parsingState)
        {
        case PARSE_START:
            if (*ptr == '<' && *(ptr + 1) == '/')
            {
                nbytes = utf8_parse(ptr, &codepoint);
                ptr += nbytes;
                index += nbytes;
                //ptr++;
                parsingState = TAG_OUT;
            }
            else if (*ptr == '<' && *(ptr + 1) != '/')
            {
                parsingState = TAG_IN;
            }
            else if (*ptr != '<')
            {
                do
                {
                    ptr--;
                    index--;
                    nbytes = utf8_parse(ptr, &codepoint);
                } while (nbytes == -1);
                parsingState = TEXT_TAG;
            }
            break;
        case TEXT_TAG:
            if (*ptr == '<')
            {
                do
                {
                    ptr--;
                    index--;
                    nbytes = utf8_parse(ptr, &codepoint);
                } while (nbytes == -1);
                traverser = nexus_node_add_child(traverser);
                nexus_string_cpy(traverser->content, buffer);
                nexus_internal_string_reset(buffer);
                parsingState = PARSE_START;
            }
            else
            {
                nexus_string_mut_data(buffer)[(*nexus_string_mut_used(buffer))++] = *ptr;
            }
            break;
        case TAG_IN:
            if (*ptr == '>')
            {
                // Add a child node to the current node, set its data to the parsed opening tag and reset buffers
                traverser = nexus_node_add_child(traverser);
                nexus_string_cpy(traverser->name, buffer);
                nexus_node_add_attribute(traverser, attrBuffer);
                traverser->type = ELEM_TYPE;
                printf("Opened <%s>\n", nexus_string_data(traverser->name));
                printf("Attributes:\n");
                nexus_internal_node_attr_foreach(traverser->attrs, print_attr);

                nexus_internal_string_reset(buffer);
                nexus_internal_string_reset(keyBuffer);
                nexus_internal_string_reset(valueBuffer);

                attrBuffer = NULL;
                parsingState = PARSE_START;
            }

            else
            {
                do
                {
                    ptr--;
                    index--;
                    nbytes = utf8_parse(ptr, &codepoint);
                } while (nbytes == -1);
                //ptr--;
                parsingState = TAG_NAME;
            }
            break;
        case TAG_NAME:
            /*
             *If encounters space, handles attribute key. Else, fills the buffer with the tag name or
            goes back to inside the tag(<>)*/
            if (*ptr == ' ')
            {
                parsingState = TAG_ATTR_NAME;
            }
            else if (*ptr == '>')
            {
                do
                {
                    ptr--;
                    index--;
                    nbytes = utf8_parse(ptr, &codepoint);
                } while (nbytes == -1);
                //ptr--;
                parsingState = TAG_IN;
            }
            else
            {
                nexus_string_mut_data(buffer)[(*nexus_string_mut_used(buffer))++] = *ptr;
            }
            break;

        case TAG_ATTR_NAME:
            if (*ptr != '=')
            {
                nexus_string_mut_data(keyBuffer)[(*nexus_string_mut_used(keyBuffer))++] = *ptr;
            }
            else
            {
                // check if attribute value starts with a double quotation mark

                if (*(ptr + 1) == '"')
                {
                    nbytes = utf8_parse(ptr, &codepoint);
                    ptr += nbytes;
                    index += nbytes;
                    // ptr++;
                    parsingState = TAG_ATTR_VALUE;
                }
                else
                {
                    fprintf(stderr, "Invalid XML document \n");
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
                nexus_internal_string_reset(keyBuffer);
                nexus_internal_string_reset(valueBuffer);
                parsingState = TAG_IN;
            }
            else
            {
                nexus_string_mut_data(valueBuffer)[(*nexus_string_mut_used(valueBuffer))++] = *ptr;
            }
            break;
        case TAG_OUT:
            if (*ptr != '>')
            {
                nexus_string_mut_data(buffer)[(*nexus_string_mut_used(buffer))++] = *ptr;
            }
            else
            {
                if (traverser->type == TEXT_TYPE && traverser->parent)
                    traverser = traverser->parent;
                printf("Comparing '%s' and '%s'\n", nexus_string_data(traverser->name), nexus_string_data(buffer));
                if (nexus_string_cmp(traverser->name, buffer) == 0)
                {
                    printf("Closing <%s>\n", nexus_string_data(traverser->name));
                    traverser = traverser->parent;
                    nexus_internal_string_reset(buffer);
                    nexus_internal_string_reset(keyBuffer);
                    nexus_internal_string_reset(valueBuffer);
                    parsingState = PARSE_START;
                }
                else
                {
                    fprintf(stderr, "Tag <%s> opened but not closed\n", nexus_string_data(traverser->name));
                    traverser = traverser->parent;
                }
            }
            break;
        }
        ptr += nbytes;
        index += nbytes;
    }
    printf("Parsing completed \n");
    return root;
}

int main()
{
    const char *buf = "Hannah Roses";
    const char *test = "<p🤷>"
        "<Child4 Name=\"sybau💔\" Class=\"Blue\"></Child4>"
        "<video Src=\"Test\" Class=\"image_large\"></video>"
        "<div width=\"34\">"
        "<img src=\"g.jpg\"></img>"
        "<book>Lord of the rings</book>"
        "</div>"
        "</p🤷>";

    string_t *str = nexus_string_wrap(test);
    string_t *substr = nexus_string_str(nexus_string_wrap(buf), nexus_string_wrap("anna"));
    printf("Substr - %s\n", nexus_string_data(substr));
    nexus_node_traverse(nexus_xml_parse(str), print_tree);

    return 0;
}