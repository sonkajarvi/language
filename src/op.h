/**
 * Copyright (c) 2026, sonkajarvi
 *
 * Licensed under the BSD 2-Clause License.
 * The full license can be found in the LICENSE.txt file.
 */

#ifndef __SRC_OP_H
#define __SRC_OP_H

/* name, string, precedence */
#define OP_LIST(x)                                                              \
    x(ADD, "+", 3)                                                              \
    x(BITWISE_AND, "&", 3)                                                      \
    x(BITWISE_NOT, "~", 0)                                                      \
    x(BITWISE_OR, "|", 3)                                                       \
    x(BITWISE_SHIFT_LEFT, "<<", 3)                                              \
    x(BITWISE_SHIFT_RIGHT, ">>", 3)                                             \
    x(BITWISE_XOR, "^", 3)                                                      \
    x(DIVIDE, "/", 4)                                                           \
    x(EQUALS, "==", 2)                                                          \
    x(GREATER_THAN, ">", 2)                                                     \
    x(GREATER_THAN_EQUALS, ">=", 2)                                             \
    x(LESS_THAN, "<", 2)                                                        \
    x(LESS_THAN_EQUALS, "<=", 2)                                                \
    x(MULTIPLY, "*", 4)                                                         \
    x(MODULO, "%", 4)                                                           \
    x(NOT_EQUALS, "!=", 2)                                                      \
    x(LOGICAL_AND, "&&", 1)                                                     \
    x(LOGICAL_NOT, "!", 0)                                                      \
    x(LOGICAL_OR, "||", 1)                                                      \
    x(SUBTRACT, "-", 3)

enum {
#define x(name, ...) OP_##name,
    OP_LIST(x)
#undef x
    OP_COUNT,
    OP_FIRST = 0,
    OP_LAST = OP_COUNT - 1
};

const char *op_to_string(int op);
int op_precedence(int op);

#endif /* __SRC_OP_H */
