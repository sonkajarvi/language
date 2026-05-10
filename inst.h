#ifndef __INST_H
#define __INST_H

#include <stdint.h>

/* (name, string) */
#define __INST_LIST(x)                                                          \
    x(NONE, "none")                                                             \
    x(SYSCALL, "syscall")                                                       \
                                                                                \
    /* Load/store */                                                            \
    x(LOAD, "load")                                                             \
    x(LOADR, "loadr")                                                           \
    x(STORE, "store")                                                           \
                                                                                \
    /* Arithmeric operations */                                                 \
    x(ADD, "add")                                                               \
    x(SUB, "sub")                                                               \
    x(MUL, "mul")                                                               \
    x(DIV, "div")                                                               \
    x(NEG, "neg")                                                               \
    x(INC, "inc")                                                               \
    x(DEC, "dec")                                                               \
                                                                                \
    /* Bitwise operations */                                                    \
    x(SHL, "shl")                                                               \
    x(SHR, "shr")                                                               \
    x(AND, "and")                                                               \
    x(OR, "or")                                                                 \
    x(XOR, "xor")                                                               \
    x(NOT, "not")                                                               \
                                                                                \
    /* Jumps */                                                                 \
    x(JUMP, "jump")                                                             \
    x(JUMP_TRUE, "jump_true")                                                   \
    x(JUMP_FALSE, "jump_false")                                                 \
    x(JUMP_EQUALS, "jump_equals")                                               \
    x(JUMP_LESS, "jump_less")                                                   \
    x(JUMP_LESS_EQUALS, "jump_less_equals")

enum {
#define X(name, ...) INST_##name,
    __INST_LIST(X)
#undef X
    INST_COUNT
};

#define __REG(reg)                                                              \
    (reg) & 0xff

#define __VAL(val)                                                              \
    ((int64_t)(val) >> 56) & 0xff,                                              \
    ((int64_t)(val) >> 48) & 0xff,                                              \
    ((int64_t)(val) >> 40) & 0xff,                                              \
    ((int64_t)(val) >> 32) & 0xff,                                              \
    ((int64_t)(val) >> 24) & 0xff,                                              \
    ((int64_t)(val) >> 16) & 0xff,                                              \
    ((int64_t)(val) >>  8) & 0xff,                                              \
    ((int64_t)(val)      ) & 0xff

#endif /* __INST_H */
