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
    char *ptr = (char *)(void *)xmldoc;
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
                // Add a child node to the curent node,set its data to the parsed opening tag and reset the buffer
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
            // check if opening or closing 
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
                printf("Comparing '%s' and '%s'\n", traverser->data, buffer);
                if (strncmp(traverser->data, buffer, sizeof(buffer) - 1) == 0)
                {
                    printf("Closing <%s>\n", traverser->data);
                    traverser = traverser->parent;
                    bufferPos = 0;
                    memset(buffer, 0, sizeof(buffer));
                    parsingState = PARSE_START;
                }
                else
                {
                    fprintf(stderr, "Tag <%s> opened but not closed\n", traverser->data);
                    traverser = traverser->parent;
                }
            }
            break;
        }

        ptr++;
    }
    printf("Parsing completed\n");
    return root;
}


int main()
{
    node_t* tree = ("<p>"
              "<Child4></Child4>"
              "<Child2>"
              "<Child3>"
              "</p>");
    return 0;
}