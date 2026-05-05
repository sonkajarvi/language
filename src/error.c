/**
 * Copyright (c) 2026, sonkajarvi
 *
 * Licensed under the BSD 2-Clause License.
 * The full license can be found in the LICENSE.txt file.
 */

#include <stdio.h>
#include <string.h>

#include "error.h"

static const char *error_strings[] = {
    "expected an expression",
    "expected ')' character",
    "expected '(' character",
    "expected 'let' keyword",
    "expected an identifier",
    "expected a type or '=' character",
    "expected a newline character",
    "expected 'if' keyword",
    "expected 'end' keyword",
    "expected a type",
    "expected ':' character",

    "out of memory",
};

static const char *get_source_line(const char *src, int line)
{
    while (src && --line) {
        src = strchr(src, '\n');
        if (src)
            src++;
    }

    return src;
}

void print_error(struct parser *parser)
{
    const char *begin, *end;

    begin = get_source_line(parser->begin, parser->line);
    end = strchr(begin, '\n') ?: strchr(begin, '\0');

    printf("in <source>:%d:%d:\n", parser->line, parser->column);
    printf("\033[1;91merror:\033[0;1m %s\033[0m\n", error_strings[parser->errno - 1]);

    printf(" %d | ", parser->line);
    fwrite(begin, end - begin, 1, stdout);
    printf("\n   |\033[%dC^\n", parser->column);
}
