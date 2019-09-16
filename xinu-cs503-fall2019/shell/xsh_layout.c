/* xsh_layout.c - xsh_layout*/

#include <xinu.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 * xsh_layout - print layout
 *------------------------------------------------------------------------
 */

// initialized global variable 
int layout_var_init = 1;

// uninitialized global variable
int layout_var_uninit;

shellcmd xsh_layout(int nargs, char *args[])
{
    // stack variable
    int layout_stack;

    kprintf("global init var addr: %x\n", &layout_var_init);
    kprintf("global uninit var addr: %x\n", &layout_var_uninit);
    kprintf("stack init var addr: %x\n", &layout_stack);

    shellcmd (*addr_xsh_layout)(int, char* []) = xsh_layout;
    kprintf("xsh_layout func addr: %x\n", addr_xsh_layout);

    return 0;
}
