#include <stdio.h>
#include <stdlib.h>

#include "vm.h"

int main()
{
    Vm v;
    //Escribe el abecedario
    Byte code[]={   o_add, 0x0, 0x1, 0x1, 0x0, 0x0, 0x0, 96,
                    o_add, 0x0, 0x2, 0x2, 0x0, 0x0, 0x0, 0x1,
                    o_add, 0x0, 0x3, 0x3, 0x0, 0x0, 0x0, 25,
                    o_add, 0x1, 0x1, 0x1, 0x2,
                    o_trap, 0x0,
                    o_loop, 0x0, 0x3, 0x0, 0x0, 0x0, 24,
                    o_halt};

    //Lee un caracter de la consola y lo almacena en el registro r1
    Byte code2[]={  o_trap, 3,
                    o_halt};

    Byte data[]= "hola mundo\0";

    init_vm(&v);
    load_code(&v, code2, 3/*24+15*/);
    load_data(&v, data, 11);
    v.regs[r1] = 0x3;
    exec_vm(&v);
    del_vm(&v);
    return 0;
}
