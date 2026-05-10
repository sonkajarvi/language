#include "src/parser.h"
#include "test.h"

#define ARRAY_SIZE(array) (sizeof(array) / sizeof((array)[0]))

/*
 * 1 ab
 * 2
 * 3 c
 * 4 de
 * 5
 */

/*                            01 2 34 567 8 */
static const char source[] = "ab\n\nc\nde\n";

#define TEST_ADVANCE(offset_, char_, line_, column_)                            \
    TEST(lexer, advance_##offset_)                                              \
    {                                                                           \
        struct parser *parser = parser_create(source, ARRAY_SIZE(source) - 1);  \
        advance_offset(parser, offset_);                                        \
        EXPECT(parser->pointer[0] == char_);                                    \
        EXPECT(parser->line == line_);                                          \
        EXPECT(parser->column == column_);                                      \
        parser_destroy(parser);                                                 \
    }

TEST_ADVANCE(0,  'a',  1, 1);
TEST_ADVANCE(1,  'b',  1, 2);
TEST_ADVANCE(2,  '\n', 1, 3);
TEST_ADVANCE(3,  '\n', 2, 1);
TEST_ADVANCE(4,  'c',  3, 1);
TEST_ADVANCE(5,  '\n', 3, 2);
TEST_ADVANCE(6,  'd',  4, 1);
TEST_ADVANCE(7,  'e',  4, 2);
TEST_ADVANCE(8,  '\n', 4, 3);
