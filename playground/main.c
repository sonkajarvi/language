#include <stdint.h>
// #include <stdio.h>

// #include "src/error.h"
#include "src/bytecode.h"
#include "src/node.h"
#include "src/parser.h"

#define ARRAY_SIZE(array) (sizeof(array) / sizeof((array)[0]))

static const char src[] = ""
    // "let a = 123\n"
    // "if a > 456\n"
    // "while a < 123\n"
    // "  # ...\n"
    // "  let a: int\n"
    // "  let b: real\n"
    // "  return a + 1\n"
    // "end\n"
    // "fun foo(a: int, b: real): int\n"
    // "  let a: int\n"
    // "  let b: real\n"
    // "end\n"
    "2 + 3"
    ;

// static uint64_t bytecode[] = {
//     INST_LOAD,          2,
//     INST_STORE,         REG(0),
//     INST_LOAD,          3,
//     INST_ADD,           REG(0),
// };

int main(void)
{
    // print_bytecode(bytecode, ARRAY_SIZE(bytecode));
    struct parser *parser;
    struct node *node;
    int ret = 0;

    parser = parser_create(src, ARRAY_SIZE(src) - 1);
    if (!parser)
        return -1;

    // node = parse_statements(parser);
    node = parse_expression(parser);

    // if (!node) {
    //     if (parser->errno != 0)
    //         print_error(parser);
    //     else
    //         printf("no nodes found\n");

    //     ret = parser->errno;
    //     goto out;
    // }

    walk_nodes(node, NULL, generate_bytecode);

    print_node(node);
    free_node(node);

// out:
    parser_destroy(parser);
    return ret;
}
