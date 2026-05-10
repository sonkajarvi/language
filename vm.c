#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#include "inst.h"
#include "vm.h"

static int __read(struct vm *vm)
{
    if (vm->pc == vm->prog_end)
        return -1;

    return *vm->pc;
}

static bool read_8(struct vm *vm, int *val)
{
    int ret;

    if ((ret = __read(vm)) == -1)
        return false;

    *val = ret;
    vm->pc++;
    return true;
}

#define int64_from_bytes(x)                                                     \
    (                                                                           \
        (((int64_t)(x)[0] & 255) << 56) |                                       \
        (((int64_t)(x)[1] & 255) << 48) |                                       \
        (((int64_t)(x)[2] & 255) << 40) |                                       \
        (((int64_t)(x)[3] & 255) << 32) |                                       \
        (((int64_t)(x)[4] & 255) << 24) |                                       \
        (((int64_t)(x)[5] & 255) << 16) |                                       \
        (((int64_t)(x)[6] & 255) <<  8) |                                       \
         ((int64_t)(x)[7] & 255)                                                \
    )

static bool read_64(struct vm *vm, int64_t *val)
{
    int ret[8];

    if (!read_8(vm, &ret[0]))
        return false;
    if (!read_8(vm, &ret[1]))
        return false;
    if (!read_8(vm, &ret[2]))
        return false;
    if (!read_8(vm, &ret[3]))
        return false;
    if (!read_8(vm, &ret[4]))
        return false;
    if (!read_8(vm, &ret[5]))
        return false;
    if (!read_8(vm, &ret[6]))
        return false;
    if (!read_8(vm, &ret[7]))
        return false;

    *val = int64_from_bytes(ret);
    return true;
}

__attribute__((unused))
static const char *insts[] = {
#define X(unused, string) string,
    __INST_LIST(X)
#undef X
};

__attribute__((unused))
static void print_vm(struct vm *vm)
{
    printf("vm: pc: %p   begin: %p   end: %p\n", vm->pc, vm->prog_begin, vm->prog_end);
    printf("vm: inst: %s   acc: %016lx\n", insts[vm->last_inst], vm->acc);
    printf("vm: r0-3: %016lx %016lx %016lx %016lx\n", vm->reg[0], vm->reg[1], vm->reg[2], vm->reg[3]);
    printf("vm: r4-7: %016lx %016lx %016lx %016lx\n", vm->reg[4], vm->reg[5], vm->reg[6], vm->reg[7]);
}

static int __NONE(struct vm *vm)
{
    (void)vm;
    return 0;
}

static int __SYSCALL(struct vm *vm)
{
    ssize_t ret;
    register int64_t r10 asm("r10") = vm->reg[4];
    register int64_t r8 asm("r8") = vm->reg[5];
    register int64_t r9 asm("r9") = vm->reg[6];

    /*
     * Syscall number: rax
     * Others:         rdi, rsi, rdx, r10, r8, r9
     * Clobbered:      rcx, r11 + memory
     */
    asm volatile
    (
        "syscall"
        : "=rax"(ret)
        : "0"(vm->reg[0]), "D"(vm->reg[1]), "S"(vm->reg[2]), "d"(vm->reg[3]),
          "r"(r10), "r"(r8), "r"(r9)
        : "rcx", "r11", "memory"
    );

    vm->acc = (int)ret;

    printf("vm: syscall %ld\n", vm->reg[0]);
    return 0;
}

static int __LOAD(struct vm *vm)
{
    int64_t val;

    if (!read_64(vm, &val))
        return -1;
    vm->acc = val;

    printf("vm: load %016lx\n", val);
    return 0;
}

static int __LOADR(struct vm *vm)
{
    int reg;

    if (!read_8(vm, &reg))
        return -1;
    vm->acc = vm->reg[reg];

    printf("vm: loadr r%u\n", reg);
    return 0;
}

static int __STORE(struct vm *vm)
{
    int reg;

    if (!read_8(vm, &reg))
        return -1;
    vm->reg[reg] = vm->acc;

    printf("vm: store r%u\n", reg);
    return 0;
}

static int __ADD(struct vm *vm)
{
    int reg;

    if (!read_8(vm, &reg))
        return -1;
    vm->acc += vm->reg[reg];

    printf("vm: add r%u\n", reg);
    return 0;
}

static int __SUB(struct vm *vm)
{
    int reg;

    if (!read_8(vm, &reg))
        return -1;
    vm->acc -= vm->reg[reg];

    printf("vm: sub r%u\n", reg);
    return 0;
}

static int __MUL(struct vm *vm)
{
    int reg;

    if (!read_8(vm, &reg))
        return -1;
    vm->acc *= vm->reg[reg];

    printf("vm: mul r%u\n", reg);
    return 0;
}

static int __DIV(struct vm *vm)
{
    int reg;

    if (!read_8(vm, &reg))
        return -1;
    vm->acc /= vm->reg[reg];

    printf("vm: div r%u\n", reg);
    return 0;
}

static int __NEG(struct vm *vm)
{
    vm->acc = -vm->acc;

    printf("vm: neg\n");
    return 0;
}

static int __INC(struct vm *vm)
{
    vm->acc++;

    printf("vm: inc\n");
    return 0;
}

static int __DEC(struct vm *vm)
{
    vm->acc--;

    printf("vm: inc\n");
    return 0;
}

static int __SHL(struct vm *vm)
{
    int reg;

    if (!read_8(vm, &reg))
        return -1;
    vm->acc <<= vm->reg[reg];

    printf("vm: shl r%u\n", reg);
    return 0;
}

static int __SHR(struct vm *vm)
{
    int reg;

    if (!read_8(vm, &reg))
        return -1;
    vm->acc >>= vm->reg[reg];

    printf("vm: shr r%u\n", reg);
    return 0;
}

static int __AND(struct vm *vm)
{
    int reg;

    if (!read_8(vm, &reg))
        return -1;
    vm->acc &= vm->reg[reg];

    printf("vm: and r%u\n", reg);
    return 0;
}

static int __OR(struct vm *vm)
{
    int reg;

    if (!read_8(vm, &reg))
        return -1;
    vm->acc |= vm->reg[reg];

    printf("vm: or r%u\n", reg);
    return 0;
}

static int __XOR(struct vm *vm)
{
    int reg;

    if (!read_8(vm, &reg))
        return -1;
    vm->acc ^= vm->reg[reg];

    printf("vm: xor r%u\n", reg);
    return 0;
}

static int __NOT(struct vm *vm)
{
    vm->acc = ~vm->acc;

    printf("vm: not\n");
    return 0;
}

static int __JUMP(struct vm *vm)
{
    int64_t addr;

    if (!read_64(vm, &addr))
        return -1;
    vm->pc = (void *)addr;

    printf("vm: jump %p\n", (void *)addr);
    return 0;
}

static int __JUMP_TRUE(struct vm *vm)
{
    int64_t addr;

    if (!read_64(vm, &addr))
        return -1;

    if (vm->acc)
        vm->pc = (void *)addr;

    printf("vm: jump_true %p\n", (void *)addr);
    return 0;
}

static int __JUMP_FALSE(struct vm *vm)
{
    int64_t addr;

    if (!read_64(vm, &addr))
        return -1;

    if (!vm->acc)
        vm->pc = (void *)addr;

    printf("vm: jump_false %p\n", (void *)addr);
    return 0;
}

static int __JUMP_EQUALS(struct vm *vm)
{
    int64_t addr;
    int reg;

    if (!read_8(vm, &reg))
        return -1;
    if (!read_64(vm, &addr))
        return -1;

    if (vm->acc == vm->reg[reg])
        vm->pc = (void *)addr;

    printf("vm: jump_equals r%u, %p\n", reg, (void *)addr);
    return 0;
}

static int __JUMP_LESS(struct vm *vm)
{
    int64_t addr;
    int reg;

    if (!read_8(vm, &reg))
        return -1;
    if (!read_64(vm, &addr))
        return -1;

    if (vm->acc < vm->reg[reg])
        vm->pc = (void *)addr;

    printf("vm: jump_less r%u, %p\n", reg, (void *)addr);
    return 0;
}

static int __JUMP_LESS_EQUALS(struct vm *vm)
{
    int64_t addr;
    int reg;

    if (!read_8(vm, &reg))
        return -1;
    if (!read_64(vm, &addr))
        return -1;

    if (vm->acc <= vm->reg[reg])
        vm->pc = (void *)addr;

    printf("vm: jump_less_equals r%u, %p\n", reg, (void *)addr);
    return 0;
}

int vm_exec(struct vm *vm)
{
    int ret, inst;

    for (;;) {
        if (vm->pc == vm->prog_end) {
            print_vm(vm);
            return 0;
        }

        if (!read_8(vm, &inst))
            return -1;

        vm->last_inst = inst;

        switch (inst) {
#define X(name, ...)                                                            \
    case INST_##name:                                                           \
        if ((ret = __##name(vm)) != 0)                                          \
            return ret;                                                         \
        break;

        __INST_LIST(X)

#undef X
        }
    }
}
