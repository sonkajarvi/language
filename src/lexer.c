/**
 * Copyright (c) 2026, sonkajarvi
 *
 * Licensed under the BSD 2-Clause License.
 * The full license can be found in the LICENSE.txt file.
 */

#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include "ast.h"
#include "token.h"

/* TODO: At the moment ASCII only; expand to UTF-8. */
static inline int peek(struct parser *parser)
{
    if (parser->ptr >= parser->end)
        return -1;
    return *parser->ptr;
}

/* TODO: At the moment ASCII only; expand to UTF-8. */
static inline int read(struct parser *parser)
{
    int ret;

    ret = peek(parser);
    if (ret != -1)
        parser->ptr++;
    return ret;
}

/*
 * ws = *ws-char
 * ws-char = %x09                  ; \t
 * ws-char =/ %x20                 ; space
 */
static inline bool is_whitespace(int ch)
{
    return ch == '\t' || ch == ' ';
}

void skip_whitespace(struct parser *parser)
{
    while (is_whitespace(peek(parser)))
        read(parser);
}

static int read_numeric_literal(struct parser *parser)
{
    int ch;

    parser->peek.begin = parser->ptr;

    if (peek(parser) == '0') {
        read(parser);
    } else {
        for (;;) {
            ch = peek(parser);
            if (ch < '0' || ch > '9')
                break;
            read(parser);
        }
    }

    parser->peek.type = TOKEN_NUMBER;
    parser->peek.end = parser->ptr;

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
    X("else", TOKEN_ELSE);
    X("end", TOKEN_END);
    X("for", TOKEN_FOR);
    X("fun", TOKEN_FUN);
    X("if", TOKEN_IF);
    X("int", TOKEN_INT);
    X("let", TOKEN_LET);
    X("real", TOKEN_REAL);
    X("string", TOKEN_STRING);
    X("while", TOKEN_WHILE);

#undef X
}

static int keyword_to_token(const char *keyword, size_t len)
{
    for (int i = 0; i < KEYWORD_COUNT; i++) {
        if (strncmp(keyword, __keywords[i].keyword, len) == 0)
            return __keywords[i].token;
    }

    return -1;
}


static int read_identifier(struct parser *parser)
{
    int type;

    parser->peek.begin = parser->ptr;

    read(parser);
    for (;;) {
        if (!is_identifier(peek(parser)))
            break;
        read(parser);
    }

    parser->peek.type = TOKEN_IDENTIFIER;
    parser->peek.end = parser->ptr;

    type = keyword_to_token(parser->peek.begin, parser->peek.end - parser->peek.begin);
    if (type != -1)
        parser->peek.type = type;

    return 0;
}

static int read_punctuator(struct parser *parser)
{
#define RETURN(type_)                                                           \
    {                                                                           \
        parser->peek.type = type_;                                              \
        return 0;                                                               \
    }

    switch (read(parser)) {
    case '+': RETURN(TOKEN_ADD);
    case '~': RETURN(TOKEN_BITWISE_NOT);
    case '^': RETURN(TOKEN_BITWISE_XOR);
    case '/': RETURN(TOKEN_DIVIDE);
    case '*': RETURN(TOKEN_MULTIPLY);
    case '%': RETURN(TOKEN_MODULO);
    case '(': RETURN(TOKEN_PAREN_LEFT);
    case ')': RETURN(TOKEN_PAREN_RIGHT);
    case '-': RETURN(TOKEN_SUBTRACT);

    case '!':
        if (peek(parser) == '=') {
            read(parser);
            RETURN(TOKEN_NOT_EQUALS);
        }
        RETURN(TOKEN_LOGICAL_NOT);

    case '&':
        if (peek(parser) == '&') {
            read(parser);
            RETURN(TOKEN_LOGICAL_AND);
        }
        RETURN(TOKEN_BITWISE_AND);

    case '=':
        if (peek(parser) == '=') {
            read(parser);
            RETURN(TOKEN_EQUALS);
        }
        RETURN(TOKEN_ASSIGN);

    case '|':
        if (peek(parser) == '|') {
            read(parser);
            RETURN(TOKEN_LOGICAL_OR);
        }
        RETURN(TOKEN_BITWISE_OR);

    case '>':
        if (peek(parser) == '=') {
            read(parser);
            RETURN(TOKEN_GREATER_THAN_EQUALS);
        } else if (peek(parser) == '>') {
            read(parser);
            RETURN(TOKEN_BITWISE_SHIFT_RIGHT);
        }
        RETURN(TOKEN_GREATER_THAN);

    case '<':
        if (peek(parser) == '=') {
            read(parser);
            RETURN(TOKEN_LESS_THAN_EQUALS);
        } else if (peek(parser) == '<') {
            read(parser);
            RETURN(TOKEN_BITWISE_SHIFT_LEFT);
        }
        RETURN(TOKEN_LESS_THAN);

    default:
        return -1;
    }

#undef RETURN
}

struct token *peek_token(struct parser *parser)
{
    struct token *token;

    token = &parser->peek;
    if (parser->has_peeked)
        goto out;

    skip_whitespace(parser);

    switch (peek(parser)) {
    default:
        token = NULL;
        break;

    case -1:
        parser->peek.type = TOKEN_EOF;
        break;

    case '0' ... '9':
        if (read_numeric_literal(parser) != 0)
            token = NULL;
        break;

    case 'A' ... 'A':
    case '_':
    case 'a' ... 'z':
        if (read_identifier(parser) != 0)
            token = NULL;
        break;

    case '+':
    case '&':
    case '|':
    case '~':
    case '>':
    case '<':
    case '^':
    case '/':
    case '=':
    case '*':
    case '%':
    case '!':
    case '(':
    case ')':
    case '-':
        if (read_punctuator(parser) != 0)
            token = NULL;
        break;
    }

out:
    parser->has_peeked = true;
    return token;
}

struct token *advance_token(struct parser *parser)
{
    if (!parser->has_peeked)
        peek_token(parser);

    memcpy(&parser->curr, &parser->peek, sizeof(parser->curr));
    parser->peek.type = TOKEN_NONE;

    parser->has_peeked = false;
    return &parser->curr;
}
