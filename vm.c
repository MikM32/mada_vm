#include "vm.h"

void f_add(Vm* vm)
{
    Byte m = FETCH(vm);
    Byte dest_reg = FETCH(vm);
    //Byte reg1 = FETCH(vm);
    Dword arg2=0;

    if(m) // add dest_reg, reg1, reg2
    {
        arg2 = FETCH(vm);
        //vm->regs[dest_reg] = vm->regs[reg1] + vm->regs[arg2];
        vm->regs[dest_reg] += vm->regs[arg2];
    }
    else // add dest_reg, reg1, imm
    {
        for(int i=0; i<4; i++)
        {
            arg2 = LSH_BYTE(arg2);
            arg2 += FETCH(vm);
        }

        //vm->regs[dest_reg] = vm->regs[reg1] + arg2;
        vm->regs[dest_reg] += arg2;

    }

    if(!vm->regs[dest_reg]) // Si el resultado es 0 coloca en 1 la bandera ZERO
    {
        TOGGLE_FLAG(vm, fzero);
    }

}

void f_xor(Vm* vm)
{
    Byte m = FETCH(vm);
    Byte dest_reg = FETCH(vm);
    Byte reg1 = FETCH(vm);
    Dword arg2=0;

    if(m) // add dest_reg, reg1, reg2
    {
        arg2 = FETCH(vm);
        vm->regs[dest_reg] = vm->regs[reg1] + vm->regs[arg2];
    }
    else // add dest_reg, reg1, imm
    {
        for(int i=0; i<4; i++)
        {
            arg2 = LSH_BYTE(arg2);
            arg2 += FETCH(vm);
        }

        vm->regs[dest_reg] = vm->regs[reg1] + arg2;
        if(!vm->regs[dest_reg])
        {
            TOGGLE_FLAG(vm, fzero);
        }
    }
}

void f_st(Vm* vm)
{
    Byte orig_reg = FETCH(vm);
    Dword addr = 0;

    for(int i=0; i<4; i++)
    {
        addr = LSH_BYTE(addr);
        addr += FETCH(vm);
    }

    vm->memory[addr] = vm->regs[orig_reg];

}

void f_ld(Vm* vm)
{
    Byte dest_reg = FETCH(vm);
    Dword addr = 0;

    if(dest_reg >= REGS_NUM)
    {
        vm_exit_error(VM_EXEC_UNKNOWN_REGISTER, "Error al ejecutar la instruccion LD: Registro de destino desconocido.\n");
    }

    for(int i=0; i<4; i++)
    {
        addr = LSH_BYTE(addr);
        addr += FETCH(vm);
    }

    vm->regs[dest_reg] = vm->memory[addr];
}

void f_loop(Vm* vm)
{
    Byte m = FETCH(vm);
    Byte times_reg = FETCH(vm);
    Dword addr=0, arg2=0;

    if(m)   //loop times_reg, reg_addr
    {
        addr = vm->regs[arg2];
    }
    else    //loop times_reg, imm_addr
    {
        for(int i=0; i<4; i++)
        {
            addr = LSH_BYTE(addr);
            addr += FETCH(vm);
        }
    }

    Dword limit = vm->regs[times_reg];
    if(limit > 0)
    {
        vm->regs[ip]=addr;
        vm->regs[times_reg]--;
    }
}

void f_trap(Vm* vm)
{
    trap_function traps[TRAPS_NUM] = {f_tputc, f_tgetc, f_tinput32, f_tprint};
    Byte trap_code = FETCH(vm);

    if(trap_code >= TRAPS_NUM)
    {
        vm_exit_error(VM_EXEC_UNKNOWN_TRAPCODE, "Error al ejecutar un TRAP: Trap code desconocido.\n");
    }

    traps[trap_code](vm);
}

void f_halt(Vm* vm)
{
    vm->flags = 1; // TOGGLE_FLAG(vm, f_halted);
}


// -->> Trap definitions <<--
void f_tputc(Vm* vm)
{
    putc(GET_BYTE(vm->regs[r1], 0), stdout);
}

void f_tgetc(Vm* vm)
{
    vm->regs[r1] = getchar();
}

void f_tinput32(Vm* vm)
{
    fscanf_s(stdin, "%d", &vm->regs[r1], sizeof(Dword));
}

void f_tprint(Vm* vm)
{
    while(vm->memory[vm->regs[r1]])
    {
        fprintf_s(stdout, "%c", vm->memory[vm->regs[r1]], sizeof(Byte));
        vm->regs[r1]++;
    }
}

void init_vm(Vm* vm)
{
    /*
    Byte* buffer = calloc(INITAL_MEM, sizeof(Byte));
    if(!buffer)
    {
        vm_exit_error(VM_OUT_OF_MEMORY, "Error al inicializar la VM: No hay memoria suficiente.\n");
    }
    vm->memory = buffer;*/
    vm->memory = NULL;
    ZERO_MEM(vm->stack, MAX_STACK_SIZE*sizeof(Byte));
    ZERO_MEM(vm->regs, REGS_NUM*sizeof(Dword));
    vm->code_size = 0;
    vm->data_size = 0;
    vm->flags = 0;
}

void load_code(Vm* vm, Byte* code, size_t code_size)
{
    vm->code_size = code_size;
    reserv_mem(vm, (vm->data_size + vm->code_size)*sizeof(Byte));
    if(!vm->memory)
    {
        vm_exit_error(VM_OUT_OF_MEMORY, "Error al cargar codigo en la VM: No hay memoria suficiente.\n");
    }
    memcpy(vm->memory, code, code_size*sizeof(Byte));
}

void load_data(Vm* vm, Byte* data, size_t data_size)
{
    if(!vm->code_size)
    {
        vm_exit_error(VM_NO_LOADED_CODE, "Error al cargar data en la VM: No se ha cargado codigo previamente.\n");
    }

    vm->data_size = data_size;
    reserv_mem(vm, (vm->data_size + vm->code_size)*sizeof(Byte));

    if(!vm->memory)
    {
        vm_exit_error(VM_OUT_OF_MEMORY, "Error al cargar data en la VM: No hay memoria suficiente.\n");
    }

    memcpy(&vm->memory[vm->code_size], data, data_size*sizeof(Byte));
}

void exec_vm(Vm* vm)
{
    if(!vm->memory)
    {
        vm_exit_error(VM_EXEC_NO_CODE, "Error al ejecutar la VM: No se ha cargado algun codigo.\n");
    }

    op_function operations[OPCODES_NUM]={f_add, f_loop, f_trap, f_halt};
    Byte opcode;

    while(!GET_FLAG(vm, fhalted))
    {
        opcode = FETCH(vm);
        if(opcode > OPCODES_NUM)
        {
            vm_exit_error(VM_EXEC_UNKNOWN_OPCODE, "Error al ejecutar la VM: Opcode desconocido.\n");
        }

        operations[opcode](vm);
    }

    vm->regs[ip]--;
}

void reserv_mem(Vm* vm, size_t mem_size)
{
    vm->memory = MADA_ALLOC(vm->memory, mem_size);
}

void reset_vm(Vm* vm)
{
    ZERO_MEM(vm->regs, REGS_NUM*sizeof(Dword));
    ZERO_MEM(vm->stack, MAX_STACK_SIZE*sizeof(Byte));
    ZERO_MEM(vm->callstack, MAX_CALLSTACK_SIZE*sizeof(Frame));
    vm->flags = 0;
}

void del_vm(Vm* vm)
{
    if(vm->memory)
    {
        vm->memory = realloc(vm->memory, 0);
    }
}
