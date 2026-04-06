/**
 * Copyright (c) 2026, sonkajarvi
 *
 * Licensed under the BSD 2-Clause License.
 * The full license can be found in the LICENSE.txt file.
 */

#include <stddef.h>
#include <stdio.h>

#include "op.h"

static const char *__op_strings[] = {
#define x(name, string, ...) string,
    OP_LIST(x)
#undef x
};

const char *op_to_string(int op)
{
    if (op < OP_FIRST || op > OP_LAST)
        return NULL;

    return __op_strings[op];
}

static const int __op_precedences[] = {
#define x(name, string, precedence) precedence,
    OP_LIST(x)
#undef x
};

int op_precedence(int op)
{
    if (op < OP_FIRST || op > OP_LAST)
        return -1;

    return __op_precedences[op];
}
