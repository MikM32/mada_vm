#include "errors.h"

void vm_exit_error(int errcode, char* msg)
{
    fprintf(stderr, msg);
    exit(errcode);
}
