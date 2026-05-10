/**
 * Copyright (c) 2026, sonkajarvi
 *
 * Licensed under the BSD 2-Clause License.
 * The full license can be found in the LICENSE.txt file.
 */

#ifndef __SRC_BYTECODE_H
#define __SRC_BYTECODE_H

#include <stddef.h>
#include <stdint.h>

#include "node.h"

#define REG(x) (x)

enum {
    INST_LOAD,
    INST_STORE,
    INST_ADD,
};

struct generator {
    
};

void print_bytecode(uint64_t *bc, size_t len);
void generate_bytecode(struct node *node, void *p);

#endif /* __SRC_BYTECODE_H */
