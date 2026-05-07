/**
 * Copyright (c) 2026, sonkajarvi
 *
 * Licensed under the BSD 2-Clause License.
 * The full license can be found in the LICENSE.txt file.
 */

#ifndef __SRC_BITS_H
#define __SRC_BITS_H

/**
 * ENUM_BITS - Generate helper values for enums
 * @prefix:     Prefix for enums
 *
 * Creates PREFIX_COUNT, PREFIX_FIRST and PREFIX_LAST for enums with linearly
 * growing values.
 */
#define ENUM_BITS(prefix)                                                       \
        prefix##_COUNT,                                                         \
        prefix##_FIRST = 0,                                                     \
        prefix##_LAST = prefix##_COUNT - 1

/**
 * ENUM_IN_RANGE - Assert that @value is in range of enum
 * @prefix:     Prefix for enums
 * @value:      The value to check
 */
#define ENUM_IN_RANGE(prefix, value)                                            \
        ((value) >= prefix##_FIRST && (value) <= prefix##_LAST)

#endif /* __SRC_BITS_H */
