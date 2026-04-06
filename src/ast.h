/**
 * Copyright (c) 2026, sonkajarvi
 *
 * Licensed under the BSD 2-Clause License.
 * The full license can be found in the LICENSE.txt file.
 */

#ifndef __SRC_AST_H
#define __SRC_AST_H

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include "token.h"

struct parser {
    const char *ptr, *end;

    struct token curr, peek;
    bool has_peeked;
};

static inline void parser_init(struct parser *parser, const char *src, size_t len)
{
    assert(parser);

    memset(parser, 0, sizeof(*parser));
    parser->ptr = src;
    parser->end = parser->ptr + len;
}

struct token *peek_token(struct parser *parser);
struct token *advance_token(struct parser *parser);

struct node *parse_expression(struct parser *parser);

#endif /* __SRC_AST_H */
