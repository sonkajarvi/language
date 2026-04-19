/**
 * Copyright (c) 2026, sonkajarvi
 *
 * Licensed under the BSD 2-Clause License.
 * The full license can be found in the LICENSE.txt file.
 */

#ifndef __SRC_TOKEN_H
#define __SRC_TOKEN_H

#include <assert.h>
#include <stddef.h>
#include <stdio.h>

#define KEYWORD_COUNT 11

#define TOKEN_LIST(x)                                                           \
    x(NONE)                     /* This is first so it's always 0. */           \
    x(EOF)                      /* End of file. */                              \
                                                                                \
    x(ADD)                      /* + */                                         \
    x(ASSIGN)                   /* = */                                         \
    x(BITWISE_AND)              /* & */                                         \
    x(BITWISE_OR)               /* | */                                         \
    x(BITWISE_NOT)              /* ~ */                                         \
    x(BITWISE_SHIFT_LEFT)       /* >> */                                        \
    x(BITWISE_SHIFT_RIGHT)      /* << */                                        \
    x(BITWISE_XOR)              /* ^ */                                         \
    x(BOOL)                     /* bool */                                      \
    x(DIVIDE)                   /* / */                                         \
    x(ELSE)                     /* else */                                      \
    x(END)                      /* end */                                       \
    x(EQUALS)                   /* == */                                        \
    x(FOR)                      /* for */                                       \
    x(FUN)                      /* fun */                                       \
    x(GREATER_THAN)             /* > */                                         \
    x(GREATER_THAN_EQUALS)      /* >= */                                        \
    x(IDENTIFIER)                                                               \
    x(IF)                       /* if */                                        \
    x(INT)                      /* int */                                       \
    x(LET)                      /* let */                                       \
    x(LESS_THAN)                /* < */                                         \
    x(LESS_THAN_EQUALS)         /* <= */                                        \
    x(MULTIPLY)                 /* * */                                         \
    x(MODULO)                   /* % */                                         \
    x(NOT_EQUALS)               /* != */                                        \
    x(NUMBER)                                                                   \
    x(LOGICAL_NOT)              /* ! */                                         \
    x(LOGICAL_AND)              /* && */                                        \
    x(LOGICAL_OR)               /* || */                                        \
    x(PAREN_LEFT)               /* ( */                                         \
    x(PAREN_RIGHT)              /* ) */                                         \
    x(REAL)                     /* real */                                      \
    x(STRING)                   /* string */                                    \
    x(SUBTRACT)                 /* - */                                         \
    x(WHILE)                    /* while */

enum {
#define x(name) TOKEN_##name,
    TOKEN_LIST(x)
#undef x
    TOKEN_COUNT,
    TOKEN_FIRST = 0,
    TOKEN_LAST = TOKEN_COUNT - 1
};

struct token {
    int type;
    const char *begin, *end;
};

static inline const char *token_to_string(struct token *token)
{
    static const char *strings[] = {
#define x(name) #name,
        TOKEN_LIST(x)
#undef x
    };

    if (!token || token->type < TOKEN_FIRST || token->type > TOKEN_LAST)
        return NULL;

    return strings[token->type];
}

static inline void print_token(struct token *token)
{
    printf("%s ", token_to_string(token));

    switch (token->type) {
    case TOKEN_NUMBER:
        fwrite(token->begin, token->end - token->begin, 1, stdout);
        break;
    }

    putchar('\n');
}

#endif /* __SRC_TOKEN_H */
