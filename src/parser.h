/**
 * Copyright (c) 2026, sonkajarvi
 *
 * Licensed under the BSD 2-Clause License.
 * The full license can be found in the LICENSE.txt file.
 */

#ifndef __SRC_PARSER_H
#define __SRC_PARSER_H

#include <stdbool.h>
#include <stddef.h>

#include "node.h"
#include "token.h"

#define PARSER_LINE_COLUMN_DEFAULT 1

enum {
    EXPECTED_EXPRESSION = 1,
    EXPECTED_RIGHT_PARENTHESIS,
    EXPECTED_LET,
    EXPECTED_IDENTIFIER,
    EXPECTED_ASSIGN,
    EXPECTED_NEWLINE,
    EXPECTED_IF,
    EXPECTED_END,

    OUT_OF_MEMORY,
};

struct parser {
    const char *begin, *end;
    const char *pointer;

    struct token tok_curr, tok_next;
    bool has_peeked;
    int tok_peeked_chars;

    int errno;
    int line, column;
};

struct parser *parser_create(const char *src, size_t len);
void parser_destroy(struct parser *parser);

void init_keywords(void);

/*
 * For lexing.
 */
#define peek(parser) peek_offset((parser), 0)
int peek_offset(struct parser *parser, size_t offset);
int advance(struct parser *parser);

struct token *peek_token(struct parser *parser);
struct token *advance_token(struct parser *parser);

bool is_comment_start(struct parser *parser);
bool is_newline_sequence(struct parser *parser);
bool is_whitespace_char(struct parser *parser);

void skip_comment(struct parser *parser);
void skip_eol(struct parser *parser);
void skip_newline(struct parser *parser);
void skip_whitespace(struct parser *parser);

/*
 * For parsing.
 */
struct node *parse_expression(struct parser *parser);
struct node *parse_statements(struct parser *parser);

struct node *parse_variable_statement(struct parser *parser);
struct node *parse_if_statement(struct parser *parser);

#endif /* __SRC_PARSER_H */
