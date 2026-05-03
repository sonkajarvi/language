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

void free_if_part(struct if_part *part)
{
    if (!part)
        return;

    free_node(part->test);
    free_node(part->stmts);
    free(part);
}

void free_elif_parts(struct elif_part *parts)
{
    struct elif_part *tmp, *tmp2;

    if (!parts)
        return;

    for (tmp = parts; tmp;) {
        tmp2 = tmp;
        tmp = tmp->next;

        free_node(tmp2->test);
        free_node(tmp2->stmts);
        free(tmp2);
    }
}

void free_else_part(struct else_part *part)
{
    if (!part)
        return;

    free_node(part->stmts);
    free(part);
}

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

    case NODE_VARIABLE_STATEMENT:
        free(((struct variable_statement *)node)->type);
        free_node(((struct variable_statement *)node)->value);
        break;

    case NODE_IF_STATEMENT:
        free_if_part(((struct if_statement *)node)->if_part);
        free_elif_parts(((struct if_statement *)node)->elif_parts);
        free_else_part(((struct if_statement *)node)->else_part);
        break;

    case NODE_WHILE_STATEMENT:
        free_node(((struct while_statement *)node)->test);
        free_node(((struct while_statement *)node)->stmts);
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

static void print_if_statement(struct if_statement *node, int indent)
{
    struct node *tmp;

    _indent(indent);
    printf("IfStatement:\n");

    /* if part */
    _indent(indent + 1);
    printf("If:\n");

    _indent(indent + 2);
    printf("Test:\n");
    _print_node(node->if_part->test, indent + 3);

    _indent(indent + 2);
    printf("Statements:\n");
    for (tmp = node->if_part->stmts; tmp; tmp = tmp->next)
        _print_node(tmp, indent + 3);

    /* elif parts */
    if (node->elif_parts) {
        for (tmp = node->elif_parts->stmts; tmp; tmp = tmp->next) {
            _indent(indent + 1);
            printf("Elif:\n");

            _indent(indent + 2);
            printf("Statements:\n");
            _print_node(tmp, indent + 3);
        }
    }

    /* else part */
    if (node->else_part) {
        _indent(indent + 1);
        printf("Else:\n");

        _indent(indent + 2);
        printf("Statements:\n");
        for (tmp = node->else_part->stmts; tmp; tmp = tmp->next)
            _print_node(tmp, indent + 3);
    }
}

static void print_while_statement(struct while_statement *node, int indent)
{
    struct node *tmp;

    _indent(indent);
    printf("WhileStatement:\n");

    /* test */
    _indent(indent + 1);
    printf("Test:\n");
    _print_node(node->test, indent + 2);

    /* statements */
    _indent(indent + 1);
    printf("Statements:\n");
    for (tmp = node->stmts; tmp; tmp = tmp->next)
        _print_node(tmp, indent + 2);
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

    case NODE_VARIABLE_STATEMENT:
        print_variable_statement((struct variable_statement *)node, indent);
        break;

    case NODE_IF_STATEMENT:
        print_if_statement((struct if_statement *)node, indent);
        break;

    case NODE_WHILE_STATEMENT:
        print_while_statement((struct while_statement *)node, indent);
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

    node = node_alloc(sizeof(*node), NODE_VARIABLE_STATEMENT);
    if (!node)
        return NULL;

    memcpy(&node->ident, ident, sizeof(*ident));
    node->type = type;
    node->value = value;

    return &node->node;
}

struct node *new_if_statement(struct if_part *if_part,
                    struct elif_part *elif_parts, struct else_part *else_part)
{
    struct if_statement *node;

    node = node_alloc(sizeof(*node), NODE_IF_STATEMENT);
    if (!node)
        return NULL;

    node->if_part = if_part;
    node->elif_parts = elif_parts;
    node->else_part = else_part;

    return &node->node;
}

struct node *new_while_statement(struct node *test, struct node *stmts)
{
    struct while_statement *node;

    node = node_alloc(sizeof(*node), NODE_WHILE_STATEMENT);
    if (!node)
        return NULL;

    node->test = test;
    node->stmts = stmts;

    return &node->node;
}
