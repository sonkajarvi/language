/**
 * Copyright (c) 2026, sonkajarvi
 *
 * Licensed under the BSD 2-Clause License.
 * The full license can be found in the LICENSE.txt file.
 */

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#include "vector.h"

static inline size_t new_capacity(size_t n)
{
    /*
     * Grows by a factor close enough to the golden ratio.
     */
    return n + (n >> 1) + (n >> 3) + 1;
}

bool __vec_try_alloc(void **pvec, size_t size)
{
    struct xd_vec_header *vec;
    size_t cap;

    if (!pvec)
        return false;

    cap = new_capacity(xd_vec_cap(*pvec));
    assert(cap > vec_cap(*pvec));

    vec = *pvec ? __vec_header(*pvec) : NULL;
    vec = realloc(vec, size * cap + sizeof(*vec));
    if (!vec)
        return false;

    /*
     * If @pvec points to a null pointer, i.e. the vector is uninitialized,
     * we must initialize the length. The capacity is always set below.
     */
    if (!(*pvec))
        vec->len = 0;

    vec->cap = cap;
    *pvec = vec + 1;

    return true;
}
