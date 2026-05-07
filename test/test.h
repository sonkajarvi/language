/**
 * Copyright (c) 2025, sonkajarvi
 *
 * Licensed under the BSD 2-Clause License.
 * The full license can be found in the LICENSE.txt file.
 */

#ifndef __TEST_TEST_H
#define __TEST_TEST_H

#include <stdint.h>
#include <stdio.h>

/**
 * TEST - Define a test case
 * @suite:      Name of the suite
 * @name:       Name of the test case
 *
 * The macro first declares a function for test case, then defines a wrapper
 * that calls the test case using gcc's constructor attribute, and finally
 * it defines the function for the test case, but leaves the body to be
 * defined by the user.
 * See: https://gcc.gnu.org/onlinedocs/gcc/Common-Attributes.html#index-constructor
 *
 * Example:
 *
 *      TEST(some_suite, some_name)
 *      {
 *          EXPECT(true);
 *          EXPECT(false);
 *      }
 */
#define TEST(suite, name)                                                       \
    void __TEST_CASE(suite, name)(struct test_result *);                        \
    __attribute__((constructor))                                                \
    void __TEST_WRAPPER(suite, name)(void)                                      \
    {                                                                           \
        struct test_case __tmp = {                                              \
            .t_suite = #suite,                                                  \
            .t_name = #name,                                                    \
            .t_path = __FILE__,                                                 \
            .t_file = __FILE_NAME__,                                            \
            .t_func = __TEST_CASE(suite, name)                                  \
        };                                                                      \
        __test_run(&__tmp);                                                     \
    }                                                                           \
    void __TEST_CASE(suite, name)(struct test_result *__TEST_RESULT)

/**
 * EXPECT - Assert that an expression is true in a test case
 * @expr:       The expression to assert
 */
#define EXPECT(expr)                                                            \
    do {                                                                        \
        if ((expr)) {                                                           \
            __TEST_RESULT->t_status = TEST_SUCCESS;                             \
        } else {                                                                \
            __TEST_RESULT->t_expr = #expr;                                      \
            __TEST_RESULT->t_line = __LINE__;                                   \
            __TEST_RESULT->t_status = TEST_FAILURE;                             \
            return;                                                             \
        }                                                                       \
    } while (0)

#define TEST_SUCCESS 0
#define TEST_FAILURE 1

#define __TEST_RESULT                    __result
#define __TEST_CASE(suite, name)         __test_case_##suite##_##name
#define __TEST_WRAPPER(suite, name)      __test_wrapper_##suite##_##name

/**
 * struct test_result - Test result structure
 * @t_expr:     The expression that was tested
 * @t_line:     Line of the failed assertion
 * @t_status:   Status of the test
 */
struct test_result {
    const char *t_expr;
    unsigned int t_line;
    int t_status;
};

/**
 * struct test_case - Test case structure
 * @t_suite:    Name of the suite
 * @t_name:     Name of the test case
 * @t_path:     File path of the test file
 * @t_file:     Name of the test file
 * @t_func:     Callback to the test case
 */
struct test_case {
    const char *t_suite;
    const char *t_name;
    const char *t_path;
    const char *t_file;
    void (*t_func)(struct test_result *);
};

extern uint64_t __passed_tests;
extern uint64_t __failed_tests;

static inline void __test_run(struct test_case *test)
{
    struct test_result result;

    test->t_func(&result);

    if (result.t_status == TEST_SUCCESS) {
        printf("\033[32m[  PASS  ]\033[0m %s:%s\n",
            test->t_suite, test->t_name);

        __passed_tests++;
    } else {
        printf("\033[31m[  FAIL  ]\033[0m %s:%s\n  In file %s, line %u:\n    Assertion failed: '%s'\n",
            test->t_suite, test->t_name, test->t_path, result.t_line, result.t_expr);

        __failed_tests++;
    }
}

#endif /* !__TEST_TEST_H */
