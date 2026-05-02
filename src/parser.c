/**
 * Copyright (c) 2026, sonkajarvi
 *
 * Licensed under the BSD 2-Clause License.
 * The full license can be found in the LICENSE.txt file.
 */

#include <stdlib.h>
#include <string.h>

#include "parser.h"

struct parser *parser_create(const char *src, size_t len)
{
    struct parser *parser;

    parser = malloc(sizeof(*parser));
    if (!parser)
        return NULL;

    memset(parser, 0, sizeof(*parser));

    parser->begin = src;
    parser->end = parser->begin + len;
    parser->pointer = parser->begin;
    parser->line = parser->column = PARSER_LINE_COLUMN_DEFAULT;

    init_keywords();

    return parser;
}

void parser_destroy(struct parser *parser)
{
    free(parser);
}
