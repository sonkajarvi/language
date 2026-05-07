/**
 * Copyright (c) 2026, sonkajarvi
 *
 * Licensed under the BSD 2-Clause License.
 * The full license can be found in the LICENSE.txt file.
 */

#include <stdint.h>
#include <stdio.h>

#include "test.h"

uint64_t __passed_tests = 0;
uint64_t __failed_tests = 0;

int main(void)
{
    printf("\n");
    printf("Passed tests: %lu\n", __passed_tests);
    printf("Failed tests: %lu\n", __failed_tests);
    printf("Total:        %lu\n", __passed_tests + __failed_tests);

    return 0;
}
