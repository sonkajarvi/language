/**
 * Copyright (c) 2026, sonkajarvi
 *
 * Licensed under the BSD 2-Clause License.
 * The full license can be found in the LICENSE.txt file.
 */

#ifndef __SRC_TYPE_H
#define __SRC_TYPE_H

enum {
    TYPE_BOOL,
    TYPE_INT,
    TYPE_REAL,
    TYPE_STRING,
    TYPE_ARRAY,
    TYPE_MAP,
};

/**
 * struct type - Structure for describing types
 * @type:   Type of type
 * @key:    Key type; used for maps
 * @value:  Value type; used for arrays and maps
 */
struct type {
    int kind;
    struct type *key, *value;
};

/**
 * type_alloc - Allocate a new type structure
 * @type:   Type of type
 * @key:    Key type; may be NULL if not required by @type
 * @value:  Value type; may be NULL if not required by @type
 *
 * Returns a new type with the given members, or a NULL pointer on error.
 */
struct type *type_alloc(int kind, struct type *key, struct type *value);
void type_free(struct type *type);
void type_print(struct type *type);

#endif /* __SRC_TYPE_H */
