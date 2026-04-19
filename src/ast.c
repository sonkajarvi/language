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
static struct node *_parse_primary_expression(struct parser *parser)
{
    struct node *expr, *ret = NULL;
    struct token *token;
    int op;

    token = advance_token(parser);

    switch (token->type) {
    case TOKEN_PAREN_LEFT:
        expr = parse_expression(parser);
        if (!expr)
            break;

        token = advance_token(parser);

        if (token->type != TOKEN_PAREN_RIGHT) {
            free_node(expr);
            parser->errno = EXPECTED_RIGHT_PARENTHESIS;
        } else {
            ret = expr;
        }
        break;

    case TOKEN_NUMBER:
        ret = new_number(token->begin, token->end);
        if (!ret)
            parser->errno = OUT_OF_MEMORY;
        break;

    case TOKEN_IDENTIFIER:
        ret = new_identifier(token->begin, token->end);
        if (!ret)
            parser->errno = OUT_OF_MEMORY;
        break;

    case TOKEN_SUBTRACT:
    case TOKEN_BITWISE_NOT:
    case TOKEN_LOGICAL_NOT:
        op = token_to_op(token);

        expr = _parse_primary_expression(parser);
        if (!expr)
            break;

        ret = new_unary_op(op, expr);
        if (!ret) {
            free_node(expr);
            parser->errno = OUT_OF_MEMORY;
        }
        break;

    default:
        parser->errno = EXPECTED_EXPRESSION;
        break;
    }

    return ret;
}

static struct node *_parse_expression(struct parser *parser, struct node *lhs, int min_precedence)
{
    struct node *rhs_outer, *rhs_inner;
    int op;

    op = token_to_op(peek_token(parser));
    /*
     * If the token is not an operator, we skip the loop and return @lhs as is.
     */
    while (op_precedence(op) >= min_precedence) {
        advance_token(parser);
        op = token_to_op(peek_token(parser));

        rhs_outer = _parse_primary_expression(parser);
        if (!rhs_outer)
            return NULL;

        while (op_precedence(token_to_op(peek_token(parser))) > op_precedence(op)) {
            rhs_inner = _parse_expression(parser, rhs_outer, op_precedence(op) + 1);
            if (!rhs_inner)
                return NULL;

            rhs_outer = rhs_inner;
        }

        lhs = new_binary_op(op, lhs, rhs_outer);
        if (!lhs) {
            free_node(rhs_outer);
            parser->errno = OUT_OF_MEMORY;
            return NULL;
        }
    }

    return lhs;
}

struct node *parse_expression(struct parser *parser)
{
    struct node *lhs, *ret;

    ret = _parse_primary_expression(parser);
    if (!ret)
        goto out;

    lhs = ret;

    ret = _parse_expression(parser, lhs, 0);
    if (!ret)
        free_node(lhs);

out:
    return ret;
}

/*
 * type = "bool" / "int" / "real" / "string"
 */
static int _parse_type(struct parser *parser)
{
    struct token *token;

    token = peek_token(parser);
    if (!token)
        goto out;

    switch (token->type) {
    /*
     * Just return the type for now.
     */
    case TOKEN_BOOL:
    case TOKEN_INT:
    case TOKEN_REAL:
    case TOKEN_STRING:
        token = advance_token(parser);
        return token->type;
    }

out:
    return -1;
}

/*
 * variable-statement = "let" identifier type
 * variable-statement =/ "let" identifier [ type ] "=" expression
 */
struct node *parse_variable_statement(struct parser *parser)
{
    struct node *ident, *expr = NULL;
    struct token *token;
    int type;

    /*
     * let
     */
    token = peek_token(parser);
    if (!token || token->type != TOKEN_LET) {
        parser->errno = EXPECTED_LET;
        return NULL;
    }
    advance_token(parser);

    /*
     * identifier
     */
    token = peek_token(parser);
    if (!token || token->type != TOKEN_IDENTIFIER) {
        parser->errno = EXPECTED_IDENTIFIER;
        return NULL;
    }

    ident = new_identifier(token->begin, token->end);
    if (!ident) {
        parser->errno = OUT_OF_MEMORY;
        return NULL;
    }
    advance_token(parser);

    /*
    * type
    * [ type ] =
    */
    type = _parse_type(parser);
    token = peek_token(parser);
    if (!token)
        return NULL;
    if (type != -1 && token->type != TOKEN_ASSIGN)
        goto out;

    token = advance_token(parser);
    if (token->type != TOKEN_ASSIGN) {
        parser->errno = EXPECTED_EQUALS;
        return NULL;
    }

    /*
     * expression
     */
    expr = parse_expression(parser);
    if (!expr)
        return NULL;

out:
    return new_variable_statement(ident, type, expr);
}
