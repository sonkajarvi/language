#ifndef __VM_H
#define __VM_H

#include <stddef.h>
#include <stdint.h>

#define REG_COUNT 8

struct vm {
    int64_t acc;
    int64_t reg[REG_COUNT];

    const uint8_t *pc;
    const uint8_t *prog_begin, *prog_end;

    uint8_t last_inst;
};

int vm_exec(struct vm *vm);

static inline void vm_set_program(struct vm *vm, const uint8_t *prog, size_t len)
{
    vm->prog_begin = prog;
    vm->prog_end = vm->prog_begin + len;

    vm->pc = vm->prog_begin;
}

#endif /* __VM_H */
