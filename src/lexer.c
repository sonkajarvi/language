/**
 * Copyright (c) 2026, sonkajarvi
 *
 * Licensed under the BSD 2-Clause License.
 * The full license can be found in the LICENSE.txt file.
 */

#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include "parser.h"
#include "token.h"

/* TODO: At the moment ASCII only; expand to UTF-8. */
int peek_offset(struct parser *parser, size_t offset)
{
    if (parser->pointer + offset >= parser->end)
        return -1;
    return parser->pointer[offset];
}

/* TODO: At the moment ASCII only; expand to UTF-8. */
int advance(struct parser *parser)
{
    int ret;

    ret = peek(parser);
    if (ret != -1) {
        parser->pointer++;

        if (ret == '\n') {
            parser->line++;
            parser->column = PARSER_LINE_COLUMN_DEFAULT;
        } else {
            parser->column++;
        }
    }

    return ret;
}

/*
 * comment-start = %x23            ; #
 */
bool is_comment_start(struct parser *parser)
{
    return peek(parser) == '#';
}

/*
 * newline = [ %x0d ] %x0a         ; \r \n
 */
bool is_newline_sequence(struct parser *parser)
{
    return peek(parser) == '\n'
        || (peek(parser) == '\r' && peek_offset(parser, 1) == '\n');
}

/*
 * newline = [ %x0d ] %x0a         ; \r \n
 */
static inline bool is_newline_char(int ch)
{
    return ch == '\r' || ch == '\n';
}

/*
 * ws-char = %x09                  ; \t
 * ws-char =/ %x20                 ; space
 */
bool is_whitespace_char(struct parser *parser)
{
    return peek(parser) == '\t' || peek(parser) == ' ';
}

/*
 * comment = comment-start *comment-char
 * comment-start = %x23            ; #
 * comment-char = %x09             ; \t
 * comment-char =/ %x20-10ffff
 */
void skip_comment(struct parser *parser)
{
    if (is_comment_start(parser)) {
        advance(parser);

        /*
         * TODO: There are a handful of other characters that we aren't
         *       checking for, but we should.
         */
        while (!is_newline_char(peek(parser)))
            advance(parser);

        parser->has_peeked = false;
    }
}

/*
 * eol = ws [ comment ]
 */
void skip_eol(struct parser *parser)
{
    skip_whitespace(parser);
    skip_comment(parser);
}

/*
 * newline = [ %x0d ] %x0a         ; \r \n
 */
void skip_newline(struct parser *parser)
{
    if (peek(parser) == '\r')
        advance(parser);
    if (peek(parser) == '\n') {
        advance(parser);
        parser->has_peeked = false;
    }
}

/*
 * ws = *ws-char
 * ws-char = %x09                  ; \t
 * ws-char =/ %x20                 ; space
 */
void skip_whitespace(struct parser *parser)
{
    while (is_whitespace_char(parser))
        advance(parser);
}

static int peek_numeric_literal(struct parser *parser)
{
    int chr, offset = 0;

    parser->tok_next.begin = parser->pointer;

    if (peek(parser) == '0') {
        offset = 1;
    } else {
        for (;;) {
            chr = peek_offset(parser, offset);
            if (chr < '0' || chr > '9')
                break;
            offset++;
        }
    }

    parser->tok_next.end = parser->tok_next.begin + offset;
    if (parser->tok_next.begin == parser->tok_next.end)
        return -1;

    parser->tok_next.type = TOKEN_NUMBER;
    parser->tok_peeked_chars = offset;

    return 0;
}

static inline bool is_identifier(int ch)
{
    switch (ch) {
    case '0' ... '9':
    case 'a' ... 'z':
    case '_':
    case 'A' ... 'A':
        return true;

    default:
        return false;
    }
}

struct keyword_pair {
    const char *keyword;
    int token;
};

static struct keyword_pair __keywords[KEYWORD_COUNT];

void init_keywords(void)
{
    int i = 0;

#define X(keyword_, token_)                                                     \
    __keywords[i].keyword = keyword_;                                           \
    __keywords[i].token = token_;                                               \
    i++;

    X("bool", TOKEN_BOOL);
    X("elif", TOKEN_ELIF);
    X("else", TOKEN_ELSE);
    X("end", TOKEN_END);
    X("for", TOKEN_FOR);
    X("fun", TOKEN_FUN);
    X("if", TOKEN_IF);
    X("int", TOKEN_INT);
    X("let", TOKEN_LET);
    X("real", TOKEN_REAL);
    X("return", TOKEN_RETURN);
    X("string", TOKEN_STRING);
    X("while", TOKEN_WHILE);

#undef X
}

static int keyword_to_token(const char *keyword, size_t len)
{
    for (int i = 0; i < KEYWORD_COUNT; i++) {
        /*
         * Make sure that the identifier we read has the same length as the
         * keyword we're comparing it against.
         */
        if (__keywords[i].keyword[len])
            continue;
        if (strncmp(keyword, __keywords[i].keyword, len) == 0)
            return __keywords[i].token;
    }

    return -1;
}

static int peek_identifier(struct parser *parser)
{
    size_t offset = 0;
    int type;

    parser->tok_next.begin = parser->pointer;

    while (is_identifier(peek_offset(parser, offset)))
        offset++;

    parser->tok_next.end = parser->tok_next.begin + offset;
    if (parser->tok_next.begin == parser->tok_next.end)
        return -1;

    parser->tok_next.type = TOKEN_IDENTIFIER;
    parser->tok_peeked_chars = offset;

    type = keyword_to_token(parser->tok_next.begin, offset);
    if (type != -1)
        parser->tok_next.type = type;

    return 0;
}

static int peek_punctuator(struct parser *parser)
{
#define X(type_, chars_)                                                   \
    do {                                                                        \
        parser->tok_next.type = (type_);                                        \
        parser->tok_peeked_chars = (chars_);                                    \
        return 0;                                                               \
    } while (0)

    switch (peek(parser)) {
    case '+': X(TOKEN_ADD, 1);
    case '~': X(TOKEN_BITWISE_NOT, 1);
    case '^': X(TOKEN_BITWISE_XOR, 1);
    case '[': X(TOKEN_BRACKET_LEFT, 1);
    case ']': X(TOKEN_BRACKET_RIGHT, 1);
    case ':': X(TOKEN_COLON, 1);
    case '/': X(TOKEN_DIVIDE, 1);
    case '*': X(TOKEN_MULTIPLY, 1);
    case '%': X(TOKEN_MODULO, 1);
    case '(': X(TOKEN_PAREN_LEFT, 1);
    case ')': X(TOKEN_PAREN_RIGHT, 1);
    case '-': X(TOKEN_SUBTRACT, 1);

    case '!':
        if (peek_offset(parser, 1) == '=')
            X(TOKEN_NOT_EQUALS, 2);
        X(TOKEN_LOGICAL_NOT, 1);

    case '&':
        if (peek_offset(parser, 1) == '&')
            X(TOKEN_LOGICAL_AND, 2);
        X(TOKEN_BITWISE_AND, 1);

    case '=':
        if (peek_offset(parser, 1) == '=')
            X(TOKEN_EQUALS, 2);
        X(TOKEN_ASSIGN, 1);

    case '|':
        if (peek_offset(parser, 1) == '|')
            X(TOKEN_LOGICAL_OR, 2);
        X(TOKEN_BITWISE_OR, 1);

    case '>':
        if (peek_offset(parser, 1) == '=')
            X(TOKEN_GREATER_THAN_EQUALS, 2);
        if (peek_offset(parser, 1) == '>')
            X(TOKEN_BITWISE_SHIFT_RIGHT, 2);
        X(TOKEN_GREATER_THAN, 1);

    case '<':
        if (peek_offset(parser, 1) == '=')
            X(TOKEN_LESS_THAN_EQUALS, 2);
        if (peek_offset(parser, 1) == '<')
            X(TOKEN_BITWISE_SHIFT_LEFT, 2);
        X(TOKEN_LESS_THAN, 1);

    default:
        return -1;
    }

#undef X
}

struct token *peek_token(struct parser *parser)
{
    struct token *tok;

    tok = &parser->tok_next;
    if (parser->has_peeked)
        goto out;

    switch (peek(parser)) {
    default:
        parser->tok_next.type = TOKEN_NONE;
        break;

    case -1:
        parser->tok_next.type = TOKEN_EOF;
        break;

    case '0' ... '9':
        if (peek_numeric_literal(parser) != 0)
            tok = NULL;
        break;

    case 'A' ... 'A':
    case '_':
    case 'a' ... 'z':
        if (peek_identifier(parser) != 0)
            tok = NULL;
        break;

    case '+':
    case '&':
    case '|':
    case '~':
    case '>':
    case '<':
    case '^':
    case '[':
    case ']':
    case ':':
    case '/':
    case '=':
    case '*':
    case '%':
    case '!':
    case '(':
    case ')':
    case '-':
        if (peek_punctuator(parser) != 0)
            tok = NULL;
        break;
    }

out:
    parser->has_peeked = true;
    return tok;
}

struct token *advance_token(struct parser *parser)
{
    if (!parser->has_peeked)
        peek_token(parser);

    memcpy(&parser->tok_curr, &parser->tok_next, sizeof(parser->tok_curr));
    parser->tok_next.type = TOKEN_NONE;

    parser->pointer += parser->tok_peeked_chars;
    parser->column += parser->tok_peeked_chars;
    parser->has_peeked = false;

    return &parser->tok_curr;
}
