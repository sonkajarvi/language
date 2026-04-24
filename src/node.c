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

#define INDENT_STRING   "  "
#define EMPTY_STRING    "--"

static inline void _indent(int indent)
{
    while (indent--)
        printf("%s", INDENT_STRING);
}

void _print_node(struct node *node, int indent);

void free_node(struct node *node)
{
    if (!node)
        return;

    switch (node->type) {
    case NODE_BINARY_OP:
        free_node(((struct binary_op *)node)->expr);
        break;

    case NODE_UNARY_OP:
        free_node(((struct unary_op *)node)->expr);
        break;

    case NODE_VARIABLE_STMT:
        free(((struct variable_statement *)node)->type);
        free_node(((struct variable_statement *)node)->value);
        break;
    }

    free_node(node->next);
    free(node);
}

const char *type_to_string(int type)
{
    switch (type) {
    case TOKEN_BOOL:    return "bool";
    case TOKEN_INT:     return "int";
    case TOKEN_REAL:    return "real";
    case TOKEN_STRING:  return "string";
    default:            return EMPTY_STRING;
    }
}

static void print_identifier(struct identifier *node, int indent)
{
    _indent(indent);
    printf("Identifier: ");

    /* identifier */
    fwrite(node->begin, node->end - node->begin, 1, stdout);
    putchar('\n');
}

static void print_number(struct number *node, int indent)
{
    _indent(indent);
    printf("Number: ");

    /* number */
    fwrite(node->begin, node->end - node->begin, 1, stdout);
    putchar('\n');
}

static void print_binary_op(struct binary_op *node, int indent)
{
    _indent(indent);
    printf("BinaryOp:\n");

    /* operator */
    _indent(indent + 1);
    printf("Op: %s\n", op_to_string(node->op));

    /* left-hand side */
    _indent(indent + 1);
    printf("Left:\n");
    _print_node(node->expr, indent + 2);

    /* right-hand side */
    _indent(indent + 1);
    printf("Right:\n");
    _print_node(node->expr->next, indent + 2);
}

static void print_unary_op(struct unary_op *node, int indent)
{
    _indent(indent);
    printf("UnaryOp:\n");

    /* operator */
    _indent(indent + 1);
    printf("Op: %s\n", op_to_string(node->op));

    /* expression */
    _indent(indent + 1);
    printf("Expression:\n");
    _print_node(node->expr, indent + 2);
}

static void print_variable_statement(struct variable_statement *node, int indent)
{
    _indent(indent);
    printf("VariableStatement:\n");

    /* identifier */
    _indent(indent + 1);
    printf("Identifier: ");
    fwrite(node->ident.begin, node->ident.end - node->ident.begin, 1, stdout);
    putchar('\n');

    /* type */
    _indent(indent + 1);
    printf("Type: %s\n", node->type ? type_to_string(node->type->type) : EMPTY_STRING);

    /* expression */
    _indent(indent + 1);
    printf("Expression:\n");
    _print_node(node->value, indent + 2);
}

void _print_node(struct node *node, int indent)
{
    if (!node) {
        _indent(indent);
        printf("%s\n", EMPTY_STRING);
        return;
    }

    switch (node->type) {
    case NODE_IDENTIFIER:
        print_identifier((struct identifier *)node, indent);
        break;

    case NODE_NUMBER:
        print_number((struct number *)node, indent);
        break;

    case NODE_BINARY_OP:
        print_binary_op((struct binary_op *)node, indent);
        break;

    case NODE_UNARY_OP:
        print_unary_op((struct unary_op *)node, indent);
        break;

    case NODE_VARIABLE_STMT:
        print_variable_statement((struct variable_statement *)node, indent);
        break;
    }
}

void print_node(struct node *node)
{
    _print_node(node, 0);
}

static inline void *node_alloc(size_t len, int type)
{
    struct node *node = calloc(1, len);

    if (node)
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
    if (!node)
        return NULL;

    node->op = op;
    node->expr = left;
    node->expr->next = right;

    return &node->node;
}

struct node *new_unary_op(int op, struct node *expr)
{
    struct unary_op *node;

    node = node_alloc(sizeof(*node), NODE_UNARY_OP);
    if (!node)
        return NULL;

    node->op = op;
    node->expr = expr;

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
