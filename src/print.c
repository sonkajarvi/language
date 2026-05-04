/**
 * Copyright (c) 2026, sonkajarvi
 *
 * Licensed under the BSD 2-Clause License.
 * The full license can be found in the LICENSE.txt file.
 */

#include <stdio.h>

#include "node.h"
#include "op.h"
#include "token.h"

#define INDENT_STRING   "  "
#define EMPTY_STRING    "---"

static void print_node_depth(struct node *node, int depth);

static inline void indent(int depth)
{
    while (depth--)
        printf("%s", INDENT_STRING);
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

static void print_identifier(struct identifier *node, int depth)
{
    indent(depth);
    printf("Identifier: ");

    /* identifier */
    fwrite(node->begin, node->end - node->begin, 1, stdout);
    putchar('\n');
}

static void print_number(struct number *node, int depth)
{
    indent(depth);
    printf("Number: ");

    /* number */
    fwrite(node->begin, node->end - node->begin, 1, stdout);
    putchar('\n');
}

static void print_binary_op(struct binary_op *node, int depth)
{
    indent(depth);
    printf("BinaryOp:\n");

    /* operator */
    indent(depth + 1);
    printf("Op: %s\n", op_to_string(node->op));

    /* left-hand side */
    indent(depth + 1);
    printf("Left:\n");
    print_node_depth(node->expr, depth + 2);

    /* right-hand side */
    indent(depth + 1);
    printf("Right:\n");
    print_node_depth(node->expr->next, depth + 2);
}

static void print_unary_op(struct unary_op *node, int depth)
{
    indent(depth);
    printf("UnaryOp:\n");

    /* operator */
    indent(depth + 1);
    printf("Op: %s\n", op_to_string(node->op));

    /* expression */
    indent(depth + 1);
    printf("Expression:\n");
    print_node_depth(node->expr, depth + 2);
}

static void print_if_statement(struct if_statement *node, int depth)
{
    struct node *tmp;

    indent(depth);
    printf("IfStatement:\n");

    /* if part */
    indent(depth + 1);
    printf("If:\n");

    indent(depth + 2);
    printf("Test:\n");
    print_node_depth(node->if_part->test, depth + 3);

    indent(depth + 2);
    printf("Statements:\n");
    for (tmp = node->if_part->stmts; tmp; tmp = tmp->next)
        print_node_depth(tmp, depth + 3);

    /* elif parts */
    if (node->elif_parts) {
        for (tmp = node->elif_parts->stmts; tmp; tmp = tmp->next) {
            indent(depth + 1);
            printf("Elif:\n");

            indent(depth + 2);
            printf("Statements:\n");
            print_node_depth(tmp, depth + 3);
        }
    }

    /* else part */
    if (node->else_part) {
        indent(depth + 1);
        printf("Else:\n");

        indent(depth + 2);
        printf("Statements:\n");
        for (tmp = node->else_part->stmts; tmp; tmp = tmp->next)
            print_node_depth(tmp, depth + 3);
    }
}

static void print_return_statement(struct return_statement *node, int depth)
{
    indent(depth);
    printf("ReturnStatement:\n");

    /* expression */
    indent(depth + 1);
    printf("Expression:\n");
    print_node_depth(node->expr, depth + 2);
}

static void print_variable_statement(struct variable_statement *node, int depth)
{
    indent(depth);
    printf("VariableStatement:\n");

    /* identifier */
    indent(depth + 1);
    printf("Identifier: ");
    fwrite(node->ident.begin, node->ident.end - node->ident.begin, 1, stdout);
    putchar('\n');

    /* type */
    indent(depth + 1);
    printf("Type: ");
    if (node->type)
        type_print(node->type);
    else
        printf("%s", EMPTY_STRING);
    printf("\n");

    /* expression */
    indent(depth + 1);
    printf("Expression:\n");
    print_node_depth(node->value, depth + 2);
}

static void print_while_statement(struct while_statement *node, int depth)
{
    struct node *tmp;

    indent(depth);
    printf("WhileStatement:\n");

    /* test */
    indent(depth + 1);
    printf("Test:\n");
    print_node_depth(node->test, depth + 2);

    /* statements */
    indent(depth + 1);
    printf("Statements:\n");
    for (tmp = node->stmts; tmp; tmp = tmp->next)
        print_node_depth(tmp, depth + 2);
}

void print_node_depth(struct node *node, int depth)
{
    if (!node) {
        indent(depth);
        printf("%s\n", EMPTY_STRING);
        return;
    }

    switch (node->type) {
    case NODE_IDENTIFIER:
        print_identifier((struct identifier *)node, depth);
        break;

    case NODE_NUMBER:
        print_number((struct number *)node, depth);
        break;

    case NODE_BINARY_OP:
        print_binary_op((struct binary_op *)node, depth);
        break;

    case NODE_UNARY_OP:
        print_unary_op((struct unary_op *)node, depth);
        break;

    case NODE_IF_STATEMENT:
        print_if_statement((struct if_statement *)node, depth);
        break;

    case NODE_RETURN_STATEMENT:
        print_return_statement((struct return_statement *)node, depth);
        break;

    case NODE_VARIABLE_STATEMENT:
        print_variable_statement((struct variable_statement *)node, depth);
        break;

    case NODE_WHILE_STATEMENT:
        print_while_statement((struct while_statement *)node, depth);
    }
}

void print_node(struct node *node)
{
    print_node_depth(node, 0);
}
