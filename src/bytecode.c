/**
 * Copyright (c) 2026, sonkajarvi
 *
 * Licensed under the BSD 2-Clause License.
 * The full license can be found in the LICENSE.txt file.
 */

#include <stdint.h>
#include <stdio.h>

#include "bytecode.h"

#define print(inst, args, ...) printf(inst "\t" args "\n", ##__VA_ARGS__)

static inline uint64_t read(uint64_t **bc)
{
    uint64_t ret = **bc;
    (*bc)++;
    return ret;
}

void print_bytecode(uint64_t *bc, size_t len)
{
    uint64_t tmp;

    for (uint64_t *end = bc + len; bc != end;) {
        switch (read(&bc)) {
        case INST_LOAD:
            tmp = read(&bc);
            print("load", "%lx", tmp);
            break;

        case INST_STORE:
            tmp = read(&bc);
            print("store", "reg%lu", tmp);
            break;

        case INST_ADD:
            tmp = read(&bc);
            print("add", "reg%lu", tmp);
            break;
        }
    }
}


static void generate_bytecode_for_literal(struct literal *node, void *p)
{
    
}

void generate_bytecode(struct node *node, void *p)
{
    printf("%d", node->type);
}
