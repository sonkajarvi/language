#include <stddef.h>
#include <string.h>

#include "src/parser.h"
#include "src/token.h"
#include "test.h"

#define ARRAY_SIZE(array) (sizeof(array) / sizeof((array)[0]))

#define TEST_TOKEN(name, source, expected)                                      \
    TEST(token, name)                                                           \
    {                                                                           \
        struct parser *parser = parser_create(source, ARRAY_SIZE(source) - 1);  \
        struct token *token = peek_token(parser);                               \
        EXPECT(token->type == expected);                                        \
        parser_destroy(parser);                                                 \
    }

/*
 * punctuators
 */
TEST_TOKEN(add,      "+", TOKEN_ADD);
TEST_TOKEN(subtract, "-", TOKEN_SUBTRACT);
TEST_TOKEN(multiply, "*", TOKEN_MULTIPLY);
TEST_TOKEN(divide,   "/", TOKEN_DIVIDE);
TEST_TOKEN(modulo,   "%", TOKEN_MODULO);

TEST_TOKEN(logical_and, "&&", TOKEN_LOGICAL_AND)
TEST_TOKEN(logical_not, "!",  TOKEN_LOGICAL_NOT)
TEST_TOKEN(logical_or,  "||", TOKEN_LOGICAL_OR)

TEST_TOKEN(bitwise_and,         "&",  TOKEN_BITWISE_AND);
TEST_TOKEN(bitwise_not,         "~",  TOKEN_BITWISE_NOT);
TEST_TOKEN(bitwise_or,          "|",  TOKEN_BITWISE_OR);
TEST_TOKEN(bitwise_shift_left,  "<<", TOKEN_BITWISE_SHIFT_LEFT);
TEST_TOKEN(bitwise_shift_right, ">>", TOKEN_BITWISE_SHIFT_RIGHT);
TEST_TOKEN(bitwise_xor,         "^",  TOKEN_BITWISE_XOR);

TEST_TOKEN(equals,              "==", TOKEN_EQUALS);
TEST_TOKEN(not_equals,          "!=", TOKEN_NOT_EQUALS);
TEST_TOKEN(greater_than,        ">",  TOKEN_GREATER_THAN);
TEST_TOKEN(greater_than_equals, ">=", TOKEN_GREATER_THAN_EQUALS);
TEST_TOKEN(less_than,           "<",  TOKEN_LESS_THAN);
TEST_TOKEN(less_than_equals,    "<=", TOKEN_LESS_THAN_EQUALS);

TEST_TOKEN(paren_left,  "(", TOKEN_PAREN_LEFT);
TEST_TOKEN(paren_right, ")", TOKEN_PAREN_RIGHT);

/*
 * keywords
 */
TEST_TOKEN(keyword_bool,   "bool",   TOKEN_BOOL);
TEST_TOKEN(keyword_else,   "else",   TOKEN_ELSE);
TEST_TOKEN(keyword_end,    "end",    TOKEN_END);
TEST_TOKEN(keyword_false,  "false",  TOKEN_FALSE);
TEST_TOKEN(keyword_for,    "for",    TOKEN_FOR);
TEST_TOKEN(keyword_fun,    "fun",    TOKEN_FUN);
TEST_TOKEN(keyword_if,     "if",     TOKEN_IF);
TEST_TOKEN(keyword_int,    "int",    TOKEN_INT);
TEST_TOKEN(keyword_let,    "let",    TOKEN_LET);
TEST_TOKEN(keyword_real,   "real",   TOKEN_REAL);
TEST_TOKEN(keyword_string, "string", TOKEN_STRING);
TEST_TOKEN(keyword_true,   "true",   TOKEN_TRUE);
TEST_TOKEN(keyword_while,  "while",  TOKEN_WHILE);

#define TEST_VALUE(name, source, expected)                                      \
    TEST(token, name)                                                           \
    {                                                                           \
        struct parser *parser = parser_create(source, ARRAY_SIZE(source) - 1);  \
        struct token *token = peek_token(parser);                               \
        EXPECT(token->type == expected);                                        \
        EXPECT(strlen(source) == token->end - token->begin);                    \
        EXPECT(strcmp(source, token->begin) == 0);                              \
        parser_destroy(parser);                                                 \
    }

/*
 * ints
 */
TEST_VALUE(int_zero, "0", TOKEN_INT_LITERAL);
TEST_VALUE(int_nonzero, "8675309", TOKEN_INT_LITERAL);

/*
 * reals
 */
TEST_VALUE(real_zero, "0.0", TOKEN_REAL_LITERAL);
TEST_VALUE(real_nonzero, "3.14159", TOKEN_REAL_LITERAL);
