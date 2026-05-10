/**
 * Copyright (c) 2026, sonkajarvi
 *
 * Licensed under the BSD 2-Clause License.
 * The full license can be found in the LICENSE.txt file.
 */

#ifndef __SRC_NODE_H
#define __SRC_NODE_H

#include <stdbool.h>
#include <stddef.h>

#include "bits.h"
#include "type.h"

/**
 * TO_NODE - Convert node implementation to base node
 * @some:       Pointer to some node implementation
 */
#define TO_NODE(some) (&((some)->node))

/**
 * FROM_NODE - Convert base node to node implementation
 * @node:       Pointer to the base node
 */
#define FROM_NODE(node) FROM_NODE_AS((node), void)

/**
 * FROM_NODE_AS - Actually convert base to node implementation
 * @node:       Pointer to the base node
 */
#define FROM_NODE_AS(node, type) ((type *)(node))

enum {
    NODE_IDENTIFIER,

    NODE_BINARY_OP,
    NODE_NUMBER,
    NODE_LITERAL,
    NODE_UNARY_OP,

    NODE_FUNCTION_STATEMENT,
    NODE_IF_STATEMENT,
    NODE_RETURN_STATEMENT,
    NODE_VARIABLE_STATEMENT,
    NODE_WHILE_STATEMENT,

    ENUM_BITS(NODE)
};

struct node {
    int type;
    struct node *next;
};

struct identifier {
    struct node node;
    const char *begin, *end;
};

struct number {
    struct node node;
    const char *begin, *end;
};

enum {
    LITERAL_INFO_TRUE,
    LITERAL_INFO_FALSE,
    LITERAL_INFO_INT,
    LITERAL_INFO_REAL,
    LITERAL_INFO_STRING,
};

struct literal {
    struct node node;

    int info;
    const char *begin, *end;
};

struct binary_op {
    struct node node;
    int op;
    struct node *expr;
};

struct unary_op {
    struct node node;
    int op;
    struct node *expr;
};

struct source_range {
    const char *begin, *end;
};

struct parameter {
    struct source_range name;
    struct type *type;
    struct parameter *next;
};

struct function_statement {
    struct node node;

    struct source_range name;
    struct parameter *params;
    struct type *type;
    struct node *stmts;
};

struct if_part {
    struct node        *test, *stmts;
};

struct elif_part {
    struct elif_part   *next;
    struct node        *test, *stmts;
};

struct else_part {
    struct node        *stmts;
};

struct if_statement {
    struct node         node;

    struct if_part     *if_part;
    struct elif_part   *elif_parts;
    struct else_part   *else_part;
};

struct return_statement {
    struct node node;

    struct node *expr;
};

struct variable_statement {
    struct node node;

    struct source_range ident;
    struct type *type;
    struct node *value;
};

struct while_statement {
    struct node node;
    struct node *test, *stmts;
};

void free_node(struct node *node);
void free_parameters(struct parameter *params);
void free_if_part(struct if_part *part);
void free_elif_parts(struct elif_part *parts);
void free_else_part(struct else_part *part);

void print_node(struct node *node);
void walk_nodes(struct node *node, void *p,
                void (*fn)(struct node *node, void *p));

struct node *new_identifier(const char *begin, const char *end);
struct node *new_number(const char *begin, const char *end);
struct node *new_binary_op(int op, struct node *left, struct node *right);
struct node *new_unary_op(int op, struct node *expr);

struct node *new_literal(int hint, const char *begin, const char *end);

static inline struct node *new_bool_literal(bool value)
{
    return new_literal(
                value ? LITERAL_INFO_TRUE : LITERAL_INFO_FALSE, NULL, NULL);
}

struct node *new_function_statement(struct source_range *name,
                                    struct parameter *params,
                                    struct type *type, struct node *stmts);
struct node *new_if_statement(struct if_part *if_part,
                    struct elif_part *elif_parts, struct else_part *else_part);
struct node *new_return_statement(struct node *expr);
struct node *new_variable_statement(struct source_range *ident,
                                    struct type *type, struct node *value);
struct node *new_while_statement(struct node *test, struct node *stmts);

#endif /* __SRC_NODE_H */
