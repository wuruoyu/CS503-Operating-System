/* dlsym.c - */

#include <xinu.h>

static void* find_sym(const char* name, Elf32_Sym* syms, int sh_size, const char* strings, char* exec)
{
    int i;
    for (i = 0; i < sh_size / sizeof(Elf32_Sym); i ++) {
        /*XDEBUG_KPRINTF("[dlsym] symbol name: %s\n", strings + syms[i].st_name);*/
        if (strcmp(name, strings + syms[i].st_name) == 0) {
            return exec + syms[i].st_value;
        }
    }
    return NULL;
}

syscall dlsym(void* handle, const char* symbol)
{
	int32 i;
	intmask mask;

	mask = disable();

    int handle_n = (int)handle;
    
    if (handle_n < 0 || handle_n >= NHANDLE) {
        XDEBUG_KPRINTF("[dlsym] handle not valid\n");
        restore(mask);
        return SYSERR;
    }

    if (handletab[handle_n].status == HANDLE_CLOSE) {
        XDEBUG_KPRINTF("[dlsym] handle not valid\n");
        restore(mask);
        return SYSERR;
    }

    if (handletab[handle_n].pid != currpid) {
        XDEBUG_KPRINTF("[dlsym] handle not valid\n");
        restore(mask);
        return SYSERR;
    }

    XDEBUG_KPRINTF("[dlsym] handle: %d\n", handle_n);
    XDEBUG_KPRINTF("[dlsym] symbol: %s\n", symbol);
    struct dl_handle_t handle_ = handletab[handle_n];


    /*XDEBUG_KPRINTF("[dlsym] handle_.sym_syms: %x\n", handle_.sym_syms);*/
    /*XDEBUG_KPRINTF("[dlsym] handle_.sym_sh_size: %d\n", handle_.sym_sh_size);*/
    /*XDEBUG_KPRINTF("[dlsym] handle_.sym_strings: %x\n", handle_.sym_strings);*/
    /*XDEBUG_KPRINTF("[dlsym] handle_.exec: %x\n", handle_.exec);*/

    char* symbol_addr;

    // find in symtab
    symbol_addr = find_sym(symbol, handle_.sym_syms, handle_.sym_sh_size, handle_.sym_strings, handle_.exec);
    if (symbol_addr == NULL) {
        XDEBUG_KPRINTF("[dlsym] We dont find %s in symtab\n", symbol);
    }
    else {
	    restore(mask);
        XDEBUG_KPRINTF("[dlsym] Find %s in symtab\n", symbol);
        return (syscall)symbol_addr;
    }

    // find in dyntab
    symbol_addr = find_sym(symbol, handle_.dyn_syms, handle_.dyn_sh_size, handle_.dyn_strings, handle_.exec);
    if (symbol_addr == NULL) {
        XDEBUG_KPRINTF("[dlsym] We dont find %s in dyntab\n", symbol);
    }
    else {
        restore(mask);
        XDEBUG_KPRINTF("[dlsym] Find %s in dyntab\n", symbol);
        return (syscall)symbol_addr;
    }

    restore(mask);
    XDEBUG_KPRINTF("[dlsym] symbol not found\n");
	return SYSERR;
}
