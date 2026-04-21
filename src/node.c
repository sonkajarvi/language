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
#include "token.h"

#define INDENT_STR "  "
#define PRINT_INDENT(indent)                                                    \
    do {                                                                        \
        for (int __i = 0; __i < indent; __i++)                                  \
            printf("%s", INDENT_STR);                                           \
    } while (0)

#define AS(T, x) ((T *)x)

void free_node(struct node *node)
{
    if (!node)
        return;

    switch (node->type) {
    case NODE_BINARY_OP:
        free_node(AS(struct binary_op, node)->expr->next);
        free_node(AS(struct binary_op, node)->expr);
        break;

    case NODE_UNARY_OP:
        free_node(AS(struct unary_op, node)->expr);
        break;

    case NODE_VARIABLE_STMT:
        free(AS(struct variable_statement, node)->type);
        free_node(AS(struct variable_statement, node)->value);
        break;
    }

    free(node);
}

const char *type_to_string(int type)
{
    switch (type) {
    case TOKEN_BOOL:    return "bool";
    case TOKEN_INT:     return "int";
    case TOKEN_REAL:    return "real";
    case TOKEN_STRING:  return "string";
    default:            return "-";
    }
}

void __print_node(struct node *node, int indent);

static void print_variable_statement(struct variable_statement *node, int indent)
{
    PRINT_INDENT(indent);
    printf("VariableStatement:\n");

    /* identifier */
    PRINT_INDENT(indent + 1);
    printf("Identifier: ");
    fwrite(node->ident.begin, node->ident.end - node->ident.begin, 1, stdout);
    putchar('\n');

    /* type */
    PRINT_INDENT(indent + 1);
    printf("Type: %s\n", node->type ? type_to_string(node->type->type) : "-");

    /* expression */
    PRINT_INDENT(indent + 1);
    printf("Expression:\n");
    __print_node(node->value, indent + 2);
}

void __print_node(struct node *node, int indent)
{
    const char *begin, *end;

    if (!node) {
        PRINT_INDENT(indent);
        printf("-\n");
        return;
    }

    switch (node->type) {
    case NODE_IDENTIFIER:
        PRINT_INDENT(indent);
        printf("Identifier: ");

        begin = AS(struct identifier, node)->begin;
        end = AS(struct identifier, node)->end;
        fwrite(begin, end - begin, 1, stdout);

        putchar('\n');
        break;

    case NODE_NUMBER:
        PRINT_INDENT(indent);
        printf("Number: ");

        begin = AS(struct number, node)->begin;
        end = AS(struct number, node)->end;
        fwrite(begin, end - begin, 1, stdout);

        putchar('\n');
        break;

    case NODE_BINARY_OP:
        PRINT_INDENT(indent);
        printf("BinaryOp:\n");

        PRINT_INDENT(indent + 1);
        printf("Op: %s\n", op_to_string(AS(struct binary_op, node)->op));

        PRINT_INDENT(indent + 1);
        printf("Left:\n");
        __print_node(AS(struct binary_op, node)->expr, indent + 2);

        PRINT_INDENT(indent + 1);
        printf("Right:\n");
        __print_node(AS(struct binary_op, node)->expr->next, indent + 2);
        break;

    case NODE_UNARY_OP:
        PRINT_INDENT(indent);
        printf("UnaryOp:\n");

        PRINT_INDENT(indent + 1);
        printf("Op: %s\n", op_to_string(AS(struct unary_op, node)->op));

        PRINT_INDENT(indent + 1);
        printf("Expression:\n");
        __print_node(AS(struct unary_op, node)->expr, indent + 2);
        break;

    case NODE_VARIABLE_STMT:
        print_variable_statement((struct variable_statement *)node, indent);
        break;
    }
}

void print_node(struct node *node)
{
    __print_node(node, 0);
}

static void *node_alloc(size_t n, int type)
{
    struct node *node;

    if (type < NODE_FIRST || type > NODE_LAST)
        return NULL;

    node = malloc(n);
    if (!node)
        return NULL;

    memset(node, 0, n);
    node->type = type;

    return node;
}

struct node *new_identifier(const char *begin, const char *end)
{
    struct identifier *node;

    node = node_alloc(sizeof(*node), NODE_IDENTIFIER);
    if (node) {
        node->begin = begin;
        node->end = end;
    }

    return &node->node;
}

struct node *new_number(const char *begin, const char *end)
{
    struct number *node;

    node = node_alloc(sizeof(*node), NODE_NUMBER);
    if (node) {
        node->begin = begin;
        node->end = end;
    }

    return &node->node;
}

struct node *new_binary_op(int op, struct node *left, struct node *right)
{
    struct binary_op *node;

    node = node_alloc(sizeof(*node), NODE_BINARY_OP);
    if (node) {
        node->op = op;
        node->expr = left;
        node->expr->next = right;
    }

    return &node->node;
}

struct node *new_unary_op(int op, struct node *expr)
{
    struct unary_op *node;

    node = node_alloc(sizeof(*node), NODE_UNARY_OP);
    if (node) {
        node->op = op;
        node->expr = expr;
    }

    return &node->node;
}

struct node *new_variable_statement(struct source_range *ident,
                                struct type *type, struct node *value)
{
    struct variable_statement *node;

    node = node_alloc(sizeof(*node), NODE_VARIABLE_STMT);
    if (!node)
        return NULL;

    memcpy(&node->ident, ident, sizeof(*ident));
    node->type = type;
    node->value = value;

    return &node->node;
}
