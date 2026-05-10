/**
 * Copyright (c) 2026, sonkajarvi
 *
 * Licensed under the BSD 2-Clause License.
 * The full license can be found in the LICENSE.txt file.
 */

#ifndef __SRC_VECTOR_H
#define __SRC_VECTOR_H

/*
 * vector.h - Type-agnostic dynamic vector implementation.
 *
 * Macros are used to achieve a generic implementation. The vector metadata is
 * stored in a header before the actual data, which allows for the vector type
 * to be a regular pointer and for the vector to be indexed directly.
 *
 * Example:
 *
 *      int *vec = NULL;
 *
 *      vec_add_tail(vec, 123);
 *      printf("%d\n", vec[0]);
 *
 *      vec_free(vec);
 *
 *
 * The following routines can be found in this file:
 *
 *      Access:
 *          vec_at
 *
 *      Iterators:
 *          vec_begin
 *          vec_end
 *          vec_for_each
 *
 *      Capacity:
 *          vec_empty
 *          vec_len
 *          vec_cap
 *
 *      Modifiers:
 *          vec_free
 *          vec_clear
 *          vec_add
 *          vec_add_tail
 *          vec_del
 *          vec_del_tail
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

/*
 * vec_at - Get element at index, with bounds checking.
 */
#define vec_at(vec, index)                                                   \
    ({                                                                          \
        (vec) && vec_len(vec) > index ? &(vec)[index] : NULL;                \
    })

/*
 * vec_begin - Get an iterator to the beginning of a vector.
 */
#define vec_begin(vec)                                                       \
    ({                                                                          \
        vec_len(vec) ? (vec) : NULL;                                         \
    })

/*
 * vec_end - Get an iterator to the end of a vector.
 */
#define vec_end(vec)                                                         \
    ({                                                                          \
        size_t __len = vec_len(vec);                                         \
        __len ? &(vec)[__len] : NULL;                                           \
    })

/**
 * vec_for_each - Iterate over a vector.
 * @vec: Vector to iterate.
 * @iter: Name for the iterator.
 */
#define vec_for_each(vec, iter)                                              \
    for (                                                                       \
        typeof(vec) __vec = (vec), iter = vec_begin(__vec);                  \
        iter != vec_end(__vec);                                              \
        iter++                                                                  \
    )

#define vec_empty(vec)   (vec_len(vec) == 0)
#define vec_len(vec)     ((size_t)((vec) ? __vec_header(vec)->len : 0))
#define vec_cap(vec)     ((size_t)((vec) ? __vec_header(vec)->cap : 0))

#define vec_free(vec)                                                           \
    do {                                                                        \
        if (vec)                                                                \
            free(__vec_header(vec));                                            \
    } while (0)

/**
 * vec_clear - Remove all elements from a vector.
 * @vec: The vector to clear.
 */
#define vec_clear(vec)                                                          \
    do {                                                                        \
        if (vec)                                                                \
            __vec_header(vec)->len = 0;                                         \
    } while (0)

/**
 * vec_add - Insert a value into a vector.
 * @vec: Vector to add to.
 * @value: The value to insert.
 * @index: The index to insert at.
 */
#define vec_add(vec, value, index)                                              \
    ({                                                                          \
        typeof(vec) __ret = NULL;                                               \
        size_t __index = (index);                                               \
        size_t __len = vec_len(vec);                                            \
                                                                                \
        if (__index == __len) {                                                 \
            __ret = vec_add_tail((vec), (value));                               \
        } else if (__index < __len) {                                           \
            if (__vec_try_alloc((void **)&(vec), sizeof(*vec))) {               \
                memmove((vec) + __index + 1, (vec) + __index,                   \
                    (__vec_header(vec)->len - __index) * sizeof(*(vec)));       \
                (vec)[__index] = (value);                                       \
                __vec_header(vec)->len++;                                       \
                __ret = (vec) + __index;                                        \
            }                                                                   \
        }                                                                       \
        __ret;                                                                  \
    })

/**
 * vec_add_tail - Append a value to a vector.
 * @vec: Vector to add to.
 * @value: The value to append.
 *
 * Returns a pointer to the added value, or a null pointer on error.
 */
#define vec_add_tail(vec, value)                                                \
    ({                                                                          \
        if (__vec_try_alloc((void **)&(vec), sizeof(*vec)))                     \
            (vec)[__vec_header(vec)->len++] = (value);                          \
        ((vec) ? &(vec)[__vec_header(vec)->len - 1] : NULL);                    \
    })

/*
 * vec_del - Remove an element from a vector.
 */
#define vec_del(vec, index)                                                     \
    do {                                                                        \
        size_t __index = (index);                                               \
        if (__index < vec_len(vec)) {                                           \
            memcpy((vec) + __index, (vec) + __index + 1,                        \
                (--__vec_header(vec)->len - __index) * sizeof(*(vec)));         \
        }                                                                       \
    } while (0)

/*
 * vec_del_tail - Remove the last element of a vector
 */
#define vec_del_tail(vec)                                                       \
    do {                                                                        \
        if (vec_len(vec))                                                       \
            __vec_header(vec)->len--;                                           \
    } while (0)

#define __vec_header(vec) ((struct vec_header *)(vec) - 1)

struct vec_header {
    size_t len, cap;
};

/**
 * __vec_try_alloc - Make sure a vector has room for more values.
 * @pvec: Pointer to the vector.
 * @size: Size of the vector's type, in bytes.
 *
 * Returns false if the allocation fails.
 */
bool __vec_try_alloc(void **pvec, size_t size);

#endif /* !__SRC_VECTOR_H */
