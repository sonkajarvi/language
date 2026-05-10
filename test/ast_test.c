#include <stddef.h>

#include "src/ast.h"
#include "test.h"

#define _TEST_PARSER_ERROR(func, suite, name, source, error)                    \
    TEST(suite, name)                                                           \
    {                                                                           \
        struct parser parser;                                                   \
        struct node *node;                                                      \
                                                                                \
        parser_init_string(&parser, source);                                    \
        node = func(&parser);                                                   \
                                                                                \
        EXPECT(node == NULL);                                                   \
        EXPECT(parser.errno == error);                                          \
    }

#define TEST_EXPRESSION_ERROR(name, source, error) \
    _TEST_PARSER_ERROR(parse_expression, expression, name, source, error)

#define TEST_VARIABLE_STATEMENT_ERROR(name, source, error) \
    _TEST_PARSER_ERROR(parse_variable_statement, variable_statement, name, source, error)

TEST_EXPRESSION_ERROR(expected_expression, "", EXPECTED_EXPRESSION);
TEST_EXPRESSION_ERROR(expected_expression_binary_operator, "1 +", EXPECTED_EXPRESSION);
TEST_EXPRESSION_ERROR(expected_expression_unary_operator, "-", EXPECTED_EXPRESSION);
TEST_EXPRESSION_ERROR(expected_right_parenthesis, "(1 + 2", EXPECTED_RIGHT_PARENTHESIS);

TEST_VARIABLE_STATEMENT_ERROR(expected_let, "", EXPECTED_LET);
TEST_VARIABLE_STATEMENT_ERROR(expected_identifier, "let", EXPECTED_IDENTIFIER);
TEST_VARIABLE_STATEMENT_ERROR(expected_assign, "let a", EXPECTED_ASSIGN);
TEST_VARIABLE_STATEMENT_ERROR(expected_expression, "let a =", EXPECTED_EXPRESSION);
