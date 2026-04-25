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

enum {
    EXPECTED_EXPRESSION = 1,
    EXPECTED_RIGHT_PARENTHESIS,
    EXPECTED_LET,
    EXPECTED_IDENTIFIER,
    EXPECTED_ASSIGN,
    EXPECTED_NEWLINE,

    OUT_OF_MEMORY,
};

struct parser {
    const char *ptr, *end;

    struct token curr, peek;
    bool has_peeked;

    int errno;
    int line, column;
};

void init_keywords(void);

static inline void parser_init(struct parser *parser, const char *src, size_t len)
{
    assert(parser);

    memset(parser, 0, sizeof(*parser));
    parser->ptr = src;
    parser->end = parser->ptr + len;
    parser->line = parser->column = 1;

    init_keywords();
}

static inline void parser_init_string(struct parser *parser, const char *src)
{
    parser_init(parser, src, strlen(src));
}

struct token *peek_token(struct parser *parser);
struct token *advance_token(struct parser *parser);

bool is_newline_sequence(struct parser *parser);
bool is_comment_start(struct parser *parser);

void skip_eol(struct parser *parser);
void skip_comment(struct parser *parser);
void skip_newline(struct parser *parser);
void skip_whitespace(struct parser *parser);

struct node *parse_expression(struct parser *parser);
struct node *parse_variable_statement(struct parser *parser);

struct node *parse_statements(struct parser *parser);

#endif /* __SRC_AST_H */
