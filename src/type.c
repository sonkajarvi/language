/**
 * Copyright (c) 2026, sonkajarvi
 *
 * Licensed under the BSD 2-Clause License.
 * The full license can be found in the LICENSE.txt file.
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "type.h"

struct type *type_alloc(int kind, struct type *key, struct type *value)
{
    struct type *type;

    if (kind == TYPE_ARRAY && !value)
        return NULL;
    if (kind == TYPE_MAP && !(key && value))
        return NULL;

    type = malloc(sizeof(*type));
    if (!type)
        return NULL;

    type->kind = kind;
    type->key = key;
    type->value = value;

    return type;
}

void type_free(struct type *type)
{
    if (!type)
        return;

    type_free(type->key);
    type_free(type->value);
    free(type);
}

void type_print(struct type *type)
{
    if (!type)
        return;

    switch (type->kind) {
    case TYPE_BOOL:
        printf("bool");
        break;

    case TYPE_INT:
        printf("int");
        break;

    case TYPE_REAL:
        printf("real");
        break;

    case TYPE_STRING:
        printf("string");
        break;

    case TYPE_ARRAY:
        printf("[]");
        type_print(type->value);
        break;

    case TYPE_MAP:
        printf("[");
        type_print(type->key);
        printf("]");
        type_print(type->value);
        break;
    }
}
