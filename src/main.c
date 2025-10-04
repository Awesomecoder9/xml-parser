#include "string.h"
#include "tree.h"
#include "utils.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
//TODO: Final version should be a dynamic library
//TODO: Implement error handling

typedef enum
{
    // Initial state
    PARSE_START,
    // When encountering '<' IN THE OPENING TAG
    TAG_IN,
    // When encountering any character except ' ' or '>' IN THE OPENING TAG
    TAG_NAME,
    // When encountering a space then a "key"= IN THE OPENING TAG
    TAG_ATTR_NAME,
    // When encountering ="value" IN THE OPENING TAG
    TAG_ATTR_VALUE,
    // When encountering "</"
    TAG_OUT
} parsing_state_t;

// XML Parsing rules
/*
Rules:
    One root element only (<data>...</data>)
    Every tag must close (</likeThis>)
    Attributes in quotes (key="value")
    Tags must nest properly (<a><b></b></a>, not <a><b></a></b>)
    Case-sensitive! <Title> ≠ <title>
*/

/*
@parameters:
    xmldoc -> C-string pointing to a valid XML document
@returns pointer to the root node of the parsed tree, NULL pointer if parsing fails

*/

node_t *xml_parse(const char *xmldoc)
{
    // state

    parsing_state_t parsingState = PARSE_START;
    node_t *root = node_create(NULL, NULL);
    node_t *traverser = root;
    register char *ptr = (char *)xmldoc;
    array_t buffer = {};
    array_t keyBuffer = {};
    array_t valueBuffer = {};
    attr_t *attrBuffer = NULL;

    // Parsing loop
    while (*ptr)
    {
        uint32_t codepoint;
        int nbytes = utf8_parse(ptr, &codepoint);

        switch (parsingState)
        {
        case PARSE_START:
            if (*ptr == '<' && *(ptr + 1) == '/')
            {
                nbytes = utf8_parse(ptr, &codepoint);
                ptr += nbytes;
                //ptr++;
                parsingState = TAG_OUT;
            }
            else if (*ptr == '<' && *(ptr + 1) != '/')
            {
                parsingState = TAG_IN;
            }
            break;
        case TAG_IN:
            if (*ptr == '>')
            {
                // Add a child node to the current node, set its data to the parsed opening tag and reset buffers
                traverser = node_add_child(traverser);
                strncpy(traverser->name, buffer.data, sizeof(buffer.data) - 1);
                node_add_attribute(traverser, attrBuffer);
                traverser->type = ELEM_TYPE;
                printf("Opened <%s>\n", traverser->name);
                printf("Attributes:\n");
                node_attr_foreach(traverser->attrs, print_attr);

                array_reset(&buffer);
                array_reset(&keyBuffer);
                array_reset(&valueBuffer);

                attrBuffer = NULL;
                parsingState = PARSE_START;
            }
            else
            {
                do
                {
                    ptr--;
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
                    nbytes = utf8_parse(ptr, &codepoint);
                } while (nbytes == -1);
                //ptr--;
                parsingState = TAG_IN;
            }
            else
            {
                buffer.data[buffer.pos++] = *ptr;
            }
            break;

        case TAG_ATTR_NAME:
            if (*ptr != '=')
            {
                keyBuffer.data[keyBuffer.pos++] = *ptr;
            }
            else
            {
                // check if attribute value starts with a double quotation mark

                if (*(ptr + 1) == '"')
                {
                    nbytes = utf8_parse(ptr, &codepoint);
                    ptr += nbytes;
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
                    attrBuffer = attr_create(keyBuffer.data, valueBuffer.data);
                }
                else
                {
                    attrBuffer->next = attr_create(keyBuffer.data, valueBuffer.data);
                }
                array_reset(&keyBuffer);
                array_reset(&valueBuffer);
                parsingState = TAG_IN;
            }
            else
            {
                valueBuffer.data[valueBuffer.pos++] = *ptr;
            }
            break;
        case TAG_OUT:
            if (*ptr != '>')
            {
                buffer.data[buffer.pos++] = *ptr;
            }
            else
            {
                printf("Comparing '%s' and '%s'\n", traverser->name, buffer.data);
                if (strncmp(traverser->name, buffer.data, sizeof(buffer.data) - 1) == 0)
                {
                    printf("Closing <%s>\n", traverser->name);
                    traverser = traverser->parent;
                    array_reset(&buffer);
                    array_reset(&keyBuffer);
                    array_reset(&valueBuffer);
                    parsingState = PARSE_START;
                }
                else
                {
                    fprintf(stderr, "Tag <%s> opened but not closed\n", traverser->name);
                    traverser = traverser->parent;
                }
            }
            break;
        }
        ptr += nbytes;
    }
    printf("Parsing completed\n");
    return root;
}

int main()
{
    node_traverse(xml_parse("<p🤷>"
                      "<Child4 Name=\"💔\" Class=\"Blue\"></Child4>"
                      "<video Src=\"Test\" Class=\"image_large\"></video>"
                      "<div width=\"34\">"
                      "<img src=\"g.jpg\"></img>"
                      "</div>"
                      "</p🤷>"), print_tree);
    return 0;
}