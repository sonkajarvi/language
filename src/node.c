/**
 * Copyright (c) 2026, sonkajarvi
 *
 * Licensed under the BSD 2-Clause License.
 * The full license can be found in the LICENSE.txt file.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "node.h"
#include "op.h"

#define INDENT_STR "  "
#define PRINT_INDENT(indent)                                                    \
    do {                                                                        \
        for (int __i = 0; __i < indent; __i++)                                  \
            printf("%s", INDENT_STR);                                           \
    } while (0)

void __print_node(struct node *node, int indent)
{
    if (!node) {
        PRINT_INDENT(indent);
        printf("<null>\n");
        return;
    }

    switch (node->type) {
    case NODE_IDENTIFIER:
        PRINT_INDENT(indent);
        printf("Identifier: ");

        fwrite(node->begin, node->end - node->begin, 1, stdout);
        putchar('\n');
        break;

    case NODE_NUMBER:
        PRINT_INDENT(indent);
        printf("Number: ");

        fwrite(node->begin, node->end - node->begin, 1, stdout);
        putchar('\n');
        break;

    case NODE_BINARY_OP:
        PRINT_INDENT(indent);
        printf("BinaryOp:\n");

        PRINT_INDENT(indent + 1);
        printf("Op: %s\n", op_to_string(node->op));

        PRINT_INDENT(indent + 1);
        printf("Left:\n");
        __print_node(node->child, indent + 2);

        PRINT_INDENT(indent + 1);
        printf("Right:\n");
        __print_node(node->child->next, indent + 2);
        break;

    case NODE_UNARY_OP:
        PRINT_INDENT(indent);
        printf("UnaryOp:\n");

        PRINT_INDENT(indent + 1);
        printf("Op: %s\n", op_to_string(node->op));

        PRINT_INDENT(indent + 1);
        printf("Expression:\n");
        __print_node(node->child, indent + 2);
        break;
    }
}

void print_node(struct node *node)
{
    __print_node(node, 0);
}

static struct node *node_alloc(int type)
{
    struct node *node;

    if (type < NODE_FIRST || type > NODE_LAST)
        return NULL;

    node = malloc(sizeof(*node));
    if (!node)
        return NULL;

    memset(node, 0, sizeof(*node));
    node->type = type;
    return node;
}

struct node *new_identifier(const char *begin, const char *end)
{
    struct node *node;

    node = node_alloc(NODE_IDENTIFIER);
    if (node) {
        node->begin = begin;
        node->end = end;
    }
    return node;
}

struct node *new_number(const char *begin, const char *end)
{
    struct node *node;

    node = node_alloc(NODE_NUMBER);
    if (node) {
        node->begin = begin;
        node->end = end;
    }
    return node;
}

struct node *new_binary_op(int op, struct node *left, struct node *right)
{
    struct node *node;

    node = node_alloc(NODE_BINARY_OP);
    if (node) {
        node->op = op;
        node->child = left;
        node->child->next = right;
    }
    return node;
}

struct node *new_unary_op(int op, struct node *expr)
{
    struct node *node;

    node = node_alloc(NODE_UNARY_OP);
    if (node) {
        node->op = op;
        node->child = expr;
    }
    return node;
}
