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

    case NODE_IF_STATEMENT:
        free_if_part(((struct if_statement *)node)->if_part);
        free_elif_parts(((struct if_statement *)node)->elif_parts);
        free_else_part(((struct if_statement *)node)->else_part);
        break;

    case NODE_RETURN_STATEMENT:
        free_node(((struct return_statement *)node)->expr);
        break;

    case NODE_VARIABLE_STATEMENT:
        free(((struct variable_statement *)node)->type);
        free_node(((struct variable_statement *)node)->value);
        break;

    case NODE_WHILE_STATEMENT:
        free_node(((struct while_statement *)node)->test);
        free_node(((struct while_statement *)node)->stmts);
        break;
    }

    free_node(node->next);
    free(node);
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

struct node *new_return_statement(struct node *expr)
{
    struct return_statement *node;

    node = node_alloc(sizeof(*node), NODE_RETURN_STATEMENT);
    if (!node)
        return NULL;

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
