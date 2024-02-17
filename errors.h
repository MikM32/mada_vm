#ifndef ERRORS_H_INCLUDED
#define ERRORS_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>

#define VM_OUT_OF_MEMORY 1
#define VM_NO_LOADED_CODE 2
#define VM_EXEC_UNKNOWN_OPCODE 3
#define VM_EXEC_NO_CODE 4
#define VM_EXEC_UNKNOWN_REGISTER 5
#define VM_EXEC_UNKNOWN_TRAPCODE 6

void vm_exit_error(int errcode, char* msg);

#endif // ERRORS_H_INCLUDED
