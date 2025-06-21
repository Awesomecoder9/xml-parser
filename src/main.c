#include "tree.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

typedef enum
{
    PARSE_START,
    PARSE_NODE_TAG_IN,
    PARSE_NODE_IN,
    PARSE_NODE_TAG_OUT
} parsing_state_t;

node_t *xml_parse(const char *xmldoc)
{
    // state
    parsing_state_t parsingState = PARSE_START;
    node_t *root = node_create(NULL, NULL);
    node_t *traverser = root;
    char *ptr = (char *)xmldoc;
    char buffer[256];
    uint16_t bufferPos = 0;
    int64_t tagid = 1;

    while (*ptr)
    {
        switch (parsingState)
        {
        case PARSE_START:
            if (*ptr == '<')
            {
                parsingState = PARSE_NODE_TAG_IN;
            }
            break;
        case PARSE_NODE_TAG_IN:
            if (*ptr != '>')
            {
                buffer[bufferPos++] = *ptr;
            }
            else
            {
                traverser = node_add_child(traverser);
                node_set_data(traverser, buffer);
                printf("Opened <%s i_id='%ld'>\n", traverser->data, tagid);
                bufferPos = 0;
                memset(buffer, 0, sizeof(buffer));
                parsingState = PARSE_NODE_IN;
                tagid++;
            }
            break;
        case PARSE_NODE_IN:
            if (*ptr == '<' && *(ptr + 1) == '/')
            {
                ptr++;
                parsingState = PARSE_NODE_TAG_OUT;
            }
            else if (*ptr == '<' && *(ptr + 1) != '/')
            {
                parsingState = PARSE_NODE_TAG_IN;
            }
            break;
        case PARSE_NODE_TAG_OUT:
            if (*ptr != '>')
            {
                buffer[bufferPos++] = *ptr;
            }
            else
            {
                printf("Closing <%s>\n", traverser->data);
                printf("Comparing '%s' and '%s'\n", traverser->data, buffer);
                if (strncmp(traverser->data, buffer, sizeof(buffer)) == 0)
                {
                    traverser = traverser->parent;
                    if (traverser && traverser->data)
                    {
                        strncpy(buffer, traverser->data, sizeof(buffer));
                    }
                }
                else
                {
                    fprintf(stderr, "Tag <%s i_id='%ld'> opened but not closed\n", traverser->data, tagid);
					int* _ = NULL;
					*_ = 4;
                }
            }
            break;
        }

        ptr++;
        // parsingState = PARSE_START;
    }
    printf("Parsing completed\n");
    return root;
}

void print(const char *str)
{
    puts(str);
}

int main()
{
    node_t *tree = xml_parse("<p>"
                             "<Child2>"
                             "<Child3>"
                             "<Child4></Child4>"
                             "</p>");
    return 0;
}