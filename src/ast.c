/**
 * Copyright (c) 2026, sonkajarvi
 *
 * Licensed under the BSD 2-Clause License.
 * The full license can be found in the LICENSE.txt file.
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ast.h"
#include "node.h"
#include "op.h"
#include "token.h"

static inline int token_to_op(struct token *token)
{
    switch (token->type) {
    /* binary */
    case TOKEN_ADD:                 return OP_ADD;
    case TOKEN_BITWISE_AND:         return OP_BITWISE_AND;
    case TOKEN_BITWISE_OR:          return OP_BITWISE_OR;
    case TOKEN_BITWISE_SHIFT_LEFT:  return OP_BITWISE_SHIFT_LEFT;
    case TOKEN_BITWISE_SHIFT_RIGHT: return OP_BITWISE_SHIFT_RIGHT;
    case TOKEN_BITWISE_XOR:         return OP_BITWISE_XOR;
    case TOKEN_DIVIDE:              return OP_DIVIDE;
    case TOKEN_EQUALS:              return OP_EQUALS;
    case TOKEN_GREATER_THAN:        return OP_GREATER_THAN;
    case TOKEN_GREATER_THAN_EQUALS: return OP_GREATER_THAN_EQUALS;
    case TOKEN_LESS_THAN:           return OP_LESS_THAN;
    case TOKEN_LESS_THAN_EQUALS:    return OP_LESS_THAN_EQUALS;
    case TOKEN_MULTIPLY:            return OP_MULTIPLY;
    case TOKEN_MODULO:              return OP_MODULO;
    case TOKEN_NOT_EQUALS:          return OP_NOT_EQUALS;
    case TOKEN_LOGICAL_AND:         return OP_LOGICAL_AND;
    case TOKEN_LOGICAL_OR:          return OP_LOGICAL_OR;

    /* binary + unary */
    case TOKEN_SUBTRACT:            return OP_SUBTRACT;

    /* unary */
    case TOKEN_BITWISE_NOT:         return OP_BITWISE_NOT;
    case TOKEN_LOGICAL_NOT:         return OP_LOGICAL_NOT;

    default:                        return -1;
    }
}

/*
 * primary = "(" expression ")"
 * primary =/ number
 * primary =/ identifier
 * primary =/ ( "-" / "~" / "!" ) primary
 */
static struct node *parse_primary(struct parser *parser)
{
    struct node *node = NULL;
    struct token *token;
    int op;

    token = advance_token(parser);
    switch (token->type) {
    case TOKEN_PAREN_LEFT:
        node = parse_expression(parser);
        assert(node);

        token = advance_token(parser);
        assert(token->type == TOKEN_PAREN_RIGHT);
        break;

    case TOKEN_NUMBER:
        node = new_number(token->begin, token->end);
        break;

    case TOKEN_IDENTIFIER:
        node = new_identifier(token->begin, token->end);
        break;

    case TOKEN_SUBTRACT:
    case TOKEN_BITWISE_NOT:
    case TOKEN_LOGICAL_NOT:
        op = token_to_op(token);
        node = new_unary_op(op, parse_primary(parser));
        assert(node);
        break;

    default:
        /* Expected '(', number or identifier. */
        break;
    }

    return node;
}

static struct node *parse_expression_1(struct parser *parser, struct node *lhs, int min_precedence)
{
    struct node *rhs;
    struct token *lookahead;
    int op;

    peek_token(parser);
    lookahead = &parser->peek;

    while (op_precedence(token_to_op(lookahead)) >= min_precedence) {
        op = token_to_op(lookahead);
        advance_token(parser);
        rhs = parse_primary(parser);

        lookahead = peek_token(parser);
        while (op_precedence(token_to_op(lookahead)) > op_precedence(op)) {
            rhs = parse_expression_1(parser, rhs, op_precedence(op) + 1);
            lookahead = peek_token(parser);
        }
        lhs = new_binary_op(op, lhs, rhs);
    }
    return lhs;
}

struct node *parse_expression(struct parser *parser)
{
    return parse_expression_1(parser, parse_primary(parser), 0);
}
