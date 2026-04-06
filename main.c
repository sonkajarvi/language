#include <stdio.h>

#include "src/ast.h"
#include "src/node.h"

#define ARRAY_SIZE(array) (sizeof(array) / sizeof((array)[0]))

static const char src[] = "1 + 2 * 3";

int main(void)
{
    struct parser parser;
    struct node *expr;

    parser_init(&parser, src, ARRAY_SIZE(src) - 1);
    printf("Source: %s\n", src);

    expr = parse_expression(&parser);
    if (!expr) {
        printf("error: failed to parse expression\n");
        return -1;
    }

    print_node(expr);
}
