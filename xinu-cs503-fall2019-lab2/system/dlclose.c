/* dlclose.c - */

#include <xinu.h>

syscall dlclose(void* handle)
{
	int32 i;
	intmask mask;

	mask = disable();

    int handle_n = (int)handle;
    XDEBUG_KPRINTF("[dlclose] handle: %d\n", handle_n);

    // check if it has been closed
    if (handle_n < 0 || handle_n >= NHANDLE) {
        XDEBUG_KPRINTF("[dlclose] handle not valid 1\n");
        restore(mask);
        return SYSERR;
    }

    if (handletab[handle_n].status == HANDLE_CLOSE) {
        XDEBUG_KPRINTF("[dlclose] handle not valid 2\n");
        restore(mask);
        return SYSERR;
    }

    // check if it is valid for current process
    if (handletab[handle_n].pid != currpid) {
        XDEBUG_KPRINTF("[dlclose] handle not valid 3\n");
        restore(mask);
        return SYSERR;
    }

    handletab[handle_n].status = DL_CLOSE;

    // TODO: filetab not close
    
    // free the buffer
	freemem(handletab[handle_n].exec, handletab[handle_n].size);

    XDEBUG_KPRINTF("[dlclose] suceed");
	restore(mask);
	return OK;
}
