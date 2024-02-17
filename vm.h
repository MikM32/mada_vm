#ifndef VM_H_INCLUDED
#define VM_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __linux__
    #include <stddef.h>
#endif

#include "bytes.h"
#include "errors.h"

#define FETCH(vm) vm->memory[vm->regs[ip]++]
#define GET_FLAG(vm, flag) GET_BIT(vm->flags, flag)
#define TOGGLE_FLAG(vm, flag) TOGGLE_BIT(vm->flags, flag)

#define INITAL_MEM 50 // 4096 Bytes = 4 Kilobytes
#define MAX_STACK_SIZE 2048
#define MAX_CALLSTACK_SIZE 50
#define MAX_LOCALS 10

#define REGS_NUM 8
#define OPCODES_NUM 18
#define TRAPS_NUM 5

typedef struct st_frame
{
    Dword ret_ip;
    Dword locals[MAX_LOCALS];
}Frame;

typedef struct st_vm
{
    Dword regs[REGS_NUM];
    Byte stack[MAX_STACK_SIZE];
    Frame callstack[MAX_CALLSTACK_SIZE];
    Byte flags;
    Byte* memory;
    Dword code_size;
    Dword data_size;
}Vm;
//typedef struct st_vm Vm;
typedef void (*op_function)(Vm* vm);
typedef void (*trap_function)(Vm* vm);


enum Opcodes
{
    o_add,
    o_loop,
    o_trap,
    o_halt

};

enum Registers
{
    ip,
    r0,
    r1,
    r2,
    r3,
    r4,
    sp,
    bp
};

enum Flags
{
    fhalted,
    fzero,
};

//operations
void f_add(Vm* vm);
void f_sub(Vm* vm);
void f_mul(Vm* vm);
void f_div(Vm* vm);
void f_lsh(Vm* vm);
void f_rsh(Vm* vm);
void f_xor(Vm* vm);
void f_jmp(Vm* vm);
void f_je(Vm* vm);
void f_jne(Vm* vm);
void f_st(Vm* vm);
void f_sti(Vm* vm);
void f_ld(Vm* vm);
void f_ldi(Vm* vm);
void f_lda(Vm* vm);
void f_push(Vm* vm);
void f_pop(Vm* vm);
void f_loop(Vm* vm);
void f_inc(Vm* vm);
void f_dec(Vm* vm);
void f_trap(Vm* vm);
void f_halt(Vm* vm);
void f_call(Vm* vm);
void f_ret(Vm* vm);

//traps
void f_tputc(Vm* vm);
void f_tgetc(Vm* vm);
void f_tinput32(Vm* vm);
void f_tprint(Vm* vm);

#ifndef MADA_ALLOC
    #define MADA_ALLOC(block, size) realloc(block, size)
#endif // MADA_ALLOC

#define ZERO_MEM(block, size) memset(block, 0, size)

void init_vm(Vm* vm);
//void load_program(Vm* vm);
void load_code(Vm* vm, Byte* code, size_t code_size);
void load_data(Vm* vm, Byte* data, size_t data_size);
void reserv_mem(Vm* vm, size_t mem_size);
void reset_vm(Vm* vm);
void exec_vm(Vm* vm);
void del_vm(Vm* vm);

#endif // VM_H_INCLUDED
