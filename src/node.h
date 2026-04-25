/**
 * Copyright (c) 2026, sonkajarvi
 *
 * Licensed under the BSD 2-Clause License.
 * The full license can be found in the LICENSE.txt file.
 */

#ifndef __SRC_NODE_H
#define __SRC_NODE_H

enum {
    NODE_IDENTIFIER,
    NODE_NUMBER,
    NODE_BINARY_OP,
    NODE_UNARY_OP,
    NODE_VARIABLE_STATEMENT,
    NODE_IF_STATEMENT,

    NODE_COUNT,
    NODE_FIRST = 0,
    NODE_LAST = NODE_COUNT - 1
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

struct type {
    int type;
};

struct variable_statement {
    struct node node;

    struct source_range ident;
    struct type *type;
    struct node *value;
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

void free_node(struct node *node);
void free_if_part(struct if_part *part);
void free_elif_parts(struct elif_part *parts);
void free_else_part(struct else_part *part);

void print_node(struct node *node);

struct node *new_identifier(const char *begin, const char *end);
struct node *new_number(const char *begin, const char *end);
struct node *new_binary_op(int op, struct node *left, struct node *right);
struct node *new_unary_op(int op, struct node *expr);

struct node *new_variable_statement(struct source_range *ident,
                                struct type *type, struct node *value);

struct node *new_if_statement(struct if_part *if_part,
                    struct elif_part *elif_parts, struct else_part *else_part);

#endif /* __SRC_NODE_H */
