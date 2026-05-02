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

#include "node.h"
#include "op.h"
#include "parser.h"
#include "token.h"

static int token_to_op(struct token *token)
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
 * primary-expression = "(" ws expression ws ")"
 * primary-expression =/ number / bool / identifier
 * primary-expression =/ ( "-" / "~" / "!" ) ws primary-expression
 */
static struct node *parse_primary_expression(struct parser *parser)
{
    struct node *expr, *ret = NULL;
    struct token *tok;
    int op;

    tok = peek_token(parser);

    switch (tok->type) {
    case TOKEN_PAREN_LEFT:
        advance_token(parser);
        skip_whitespace(parser);

        expr = parse_expression(parser);
        if (!expr)
            break;

        skip_whitespace(parser);

        tok = peek_token(parser);
        if (tok->type != TOKEN_PAREN_RIGHT) {
            free_node(expr);
            parser->errno = EXPECTED_RIGHT_PARENTHESIS;
        } else {
            advance_token(parser);
            ret = expr;
        }
        break;

    case TOKEN_NUMBER:
        advance_token(parser);

        ret = new_number(tok->begin, tok->end);
        if (!ret)
            parser->errno = OUT_OF_MEMORY;

        break;

    case TOKEN_IDENTIFIER:
        advance_token(parser);

        ret = new_identifier(tok->begin, tok->end);
        if (!ret)
            parser->errno = OUT_OF_MEMORY;

        break;

    case TOKEN_SUBTRACT:
    case TOKEN_BITWISE_NOT:
    case TOKEN_LOGICAL_NOT:
        op = token_to_op(tok);
        advance_token(parser);

        skip_whitespace(parser);

        expr = parse_primary_expression(parser);
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

static inline int peek_op(struct parser *parser)
{
    return token_to_op(peek_token(parser));
}

static struct node *_parse_expression(struct parser *parser, struct node *lhs, int min_precedence)
{
    struct node *rhs_outer, *rhs_inner;
    int op;

    skip_whitespace(parser);
    op = peek_op(parser);

    /*
     * If the token is not an operator, we skip the loop and return @lhs as is.
     */
    while (op_precedence(peek_op(parser)) >= min_precedence) {
        advance_token(parser);
        skip_whitespace(parser);

        rhs_outer = parse_primary_expression(parser);
        if (!rhs_outer)
            return NULL;

        skip_whitespace(parser);

        while (op_precedence(peek_op(parser)) > op_precedence(op)) {
            rhs_inner = _parse_expression(parser,
                                    rhs_outer, op_precedence(op) + 1);
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

        op = peek_op(parser);
    }

    return lhs;
}

struct node *parse_expression(struct parser *parser)
{
    struct node *lhs, *ret;

    ret = parse_primary_expression(parser);
    if (!ret)
        goto out;

    lhs = ret;
    skip_whitespace(parser);

    ret = _parse_expression(parser, lhs, 0);
    if (!ret)
        free_node(lhs);

out:
    return ret;
}

/*
 * type = "bool" / "int" / "real" / "string"
 */
static struct type *parse_type(struct parser *parser)
{
    struct token *tok;
    struct type *type;

    tok = peek_token(parser);
    if (!tok)
        return NULL;

    switch (tok->type) {
    case TOKEN_BOOL:
    case TOKEN_INT:
    case TOKEN_REAL:
    case TOKEN_STRING:
        tok = advance_token(parser);

        type = malloc(sizeof(*type));
        if (!type) {
            parser->errno = OUT_OF_MEMORY;
            return NULL;
        }

        type->type = tok->type;
        return type;

    default:
        return NULL;
    }
}

/*
 * variable-statement = let-keyword ws identifier ws type
 * variable-statement =/ let-keyword ws identifier ws [ type ws ] "=" ws expression
 */
struct node *parse_variable_statement(struct parser *parser)
{
    struct token *tok;
    struct source_range ident;
    struct type *type;
    struct node *node, *value = NULL;

    /*
     * Skip the "let" token unconditionally; this function is only called if
     * the peeked token was "let", thus we can safely discard it here.
     */
    advance_token(parser);
    skip_whitespace(parser);

    /* identifier */
    tok = peek_token(parser);
    if (tok->type != TOKEN_IDENTIFIER) {
        parser->errno = EXPECTED_IDENTIFIER;
        return NULL;
    }

    ident.begin = tok->begin;
    ident.end = tok->end;

    advance_token(parser);
    skip_whitespace(parser);

    /* type */
    type = parse_type(parser);
    if (!type && parser->errno != 0)
        return NULL;

    skip_whitespace(parser);

    tok = peek_token(parser);
    if (tok->type != TOKEN_ASSIGN) {
        if (!type) {
            /*
             * We didn't get a type or an assignment token; we cannot continue
             * from here anymore.
             */
            parser->errno = EXPECTED_ASSIGN;
            return NULL;
        } else {
            /*
             * We got a type but not an assignment token; jump to the end.
             */
            goto out;
        }
    }

    advance_token(parser);
    skip_whitespace(parser);

    /* value */
    value = parse_expression(parser);
    if (!value) {
        free(type);
        return NULL;
    }

out:
    node = new_variable_statement(&ident, type, value);
    if (node)
        return node;

    free(type);
    free_node(value);

    parser->errno = OUT_OF_MEMORY;
    return NULL;
}

/*
 * statements-part = ws ( [ comment ] / ( statement eol ) )
 *
 * statement = for-statement
 * statement =/ function-statement
 * statement =/ if-statement
 * statement =/ return-statement
 * statement =/ variable-statement
 * statement =/ while-statement
 */
static struct node *parse_statements_part(struct parser *parser)
{
    struct node *stmt;
    struct token *tok;

    skip_whitespace(parser);

    if (is_comment_start(parser))
        skip_comment(parser);

    tok = peek_token(parser);

    switch (tok->type) {
    case TOKEN_LET:
        stmt = parse_variable_statement(parser);
        break;

    case TOKEN_IF:
        stmt = parse_if_statement(parser);
        break;

    default:
        parser->errno = 0;
        return NULL;
    }

    return stmt;
}

/*
 * statements = statements-part *( newline statements-part )
 */
struct node *parse_statements(struct parser *parser)
{
    struct node *stmts, **tmp;

    stmts = parse_statements_part(parser);

    if (stmts) {
        tmp = &stmts->next;

        for (;;) {
            if (!is_newline_sequence(parser)) {
                parser->errno = EXPECTED_NEWLINE;
                free_node(stmts);
                return NULL;
            }
            skip_newline(parser);

            *tmp = parse_statements_part(parser);
            if (!*tmp)
                break;

            tmp = &((*tmp)->next);
        }
    }

    if (parser->errno != 0)
        return NULL;

    return stmts;
}

/*
 * if-part = if-keyword ws expression eol newline statements
 */
static struct if_part *parse_if_part(struct parser *parser)
{
    struct if_part *part;
    struct node *test, *stmts = NULL;

    /* if */
    advance_token(parser);
    skip_whitespace(parser);

    /* expression */
    test = parse_expression(parser);
    if (!test)
        return NULL;

    skip_eol(parser);

    /* newline */
    if (!is_newline_sequence(parser)) {
        parser->errno = EXPECTED_NEWLINE;
        goto err;
    }
    skip_newline(parser);

    /* statements */
    stmts = parse_statements(parser);
    if (!stmts && parser->errno != 0)
        goto err;

    part = calloc(1, sizeof(*part));
    if (!part) {
        parser->errno = OUT_OF_MEMORY;
        goto err;
    }
    part->test = test;
    part->stmts = stmts;
    return part;

err:
    free_node(test);
    free_node(stmts);
    return NULL;
}

/*
 * elif-part = elif-keyword ws expression eol newline statements
 */
static struct elif_part *parse_elif_part(struct parser *parser)
{
    struct elif_part *part;
    struct node *test, *stmts = NULL;

    /* elif */
    advance_token(parser);
    skip_whitespace(parser);

    /* expression */
    test = parse_expression(parser);
    if (!test)
        return NULL;

    skip_eol(parser);

    /* newline */
    if (!is_newline_sequence(parser)) {
        parser->errno = EXPECTED_NEWLINE;
        goto err;
    }
    skip_newline(parser);

    /* statements */
    stmts = parse_statements(parser);
    if (!stmts && parser->errno != 0)
        goto err;

    part = calloc(1, sizeof(*part));
    if (!part) {
        parser->errno = OUT_OF_MEMORY;
        goto err;
    }
    part->test = test;
    part->stmts = stmts;
    return part;

err:
    free_node(test);
    free_node(stmts);
    return NULL;
}

static struct elif_part *parse_elif_parts(struct parser *parser)
{
    struct elif_part *parts, **tmp;

    /* elif */
    parts = parse_elif_part(parser);

    if (parts) {
        tmp = &parts->next;

        for (;;) {
            if (peek_token(parser)->type != TOKEN_ELIF)
                break;

            *tmp = parse_elif_part(parser);
            if (!*tmp)
                break;
            tmp = &((*tmp)->next);
        }
    }

    if (parser->errno != 0)
        return NULL;

    return parts;
}

/*
 * else-part = else-keyword eol newline statements
 */
static struct else_part *parse_else_part(struct parser *parser)
{
    struct else_part *part;
    struct node *stmts;

    /* else */
    advance_token(parser);
    skip_whitespace(parser);
    skip_eol(parser);

    /* newline */
    if (!is_newline_sequence(parser)) {
        parser->errno = EXPECTED_NEWLINE;
        return NULL;
    }
    skip_newline(parser);

    /* statements */
    stmts = parse_statements(parser);
    if (!stmts && parser->errno != 0)
        return NULL;

    part = calloc(1, sizeof(*part));
    if (!part) {
        free_node(stmts);
        parser->errno = OUT_OF_MEMORY;
        return NULL;
    }

    part->stmts = stmts;
    return part;
}

/*
 * if-statement = if-part *( elif-part ) [ else-part ] ws end-keyword
 * if-part = if-keyword ws expression eol newline statements
 * elif-part = elif-keyword ws expression eol newline statements
 * else-part = else-keyword eol newline statements
 */
struct node *parse_if_statement(struct parser *parser)
{
    struct if_part *if_part;
    struct elif_part *elif_parts = NULL;
    struct else_part *else_part = NULL;
    struct node *ret;
    struct token *tok;

    /* if part */
    tok = peek_token(parser);
    if (tok->type != TOKEN_IF) {
        parser->errno = EXPECTED_IF;
        return NULL;
    }
    if_part = parse_if_part(parser);
    if (!if_part)
        return NULL;

    /* elif parts */
    if (peek_token(parser)->type == TOKEN_ELIF) {
        elif_parts = parse_elif_parts(parser);
        if (!elif_parts && parser->errno != 0)
            goto err;
    }

    /* else part */
    if (peek_token(parser)->type == TOKEN_ELSE) {
        else_part = parse_else_part(parser);
        if (!else_part && parser->errno != 0)
            goto err;
    }

    skip_whitespace(parser);

    /* end */
    tok = peek_token(parser);
    if (tok->type != TOKEN_END) {
        parser->errno = EXPECTED_END;
        return NULL;
    }
    advance_token(parser);

    ret = new_if_statement(if_part, elif_parts, else_part);
    if (ret)
        return ret;

    parser->errno = OUT_OF_MEMORY;

err:
    free_if_part(if_part);
    free_elif_parts(elif_parts);
    free_else_part(else_part);
    return NULL;
}
