#include <xinu.h>

extern int error_code;
unsigned long tmp;

unsigned long read_cr2(void) {
    intmask mask;
    mask = disable();
    unsigned long local_tmp;

    asm("pushl %eax");
    asm("movl %cr2, %eax");
    asm("movl %eax, tmp");
    asm("popl %eax");

    local_tmp = tmp;

    restore(mask);

    return local_tmp;
}

void pagehandler(void) {
    struct procent* prptr;
    pd_t* pdptr;
    pt_t* ptptr;
    char* faulted_addr;
    int faulted_addr_int;
    int pd_idx;
    int pt_idx;
    int offset;

    faulted_addr = read_cr2();
    faulted_addr_int = (int)faulted_addr;

    prptr = &proctab[currpid];
    pdptr = prptr->prpdptr;

    // check if faulted_addr valid
    if (faulted_addr_int < 0x1000000 
            || faulted_addr_int > (0x1000000 + (prptr->vsize) * NBPG)) {
        XDEBUG_KPRINTF("[pagehandler] faulted_addr error\n");
        kill(currpid);
        return;
    }

    pd_idx = (faulted_addr_int >> 22);
    pt_idx = ((faulted_addr_int & 0x3FF000) >> 12);
    offset = (faulted_addr_int & 0xFFF);
    XDEBUG_KPRINTF("[pagehandler] error code: %d, faulted_addr: %x, pd_idx: %d, pt_idx: %d, offset: %d\n", 
            error_code, faulted_addr, pd_idx, pt_idx, offset);

    // pt not present
    if ((pdptr + pd_idx)->pd_pres == 0) {

        XDEBUG_KPRINTF("[pagehandler] pt not present\n");

        // pt not exist

        // pt in bs

        return;
    }

    if ((pdptr + pd_idx)->pd_base == 0) {
        XDEBUG_KPRINTF("[pagehandler] should not equal to 0\n");
        return;
    }

    ptptr = ((pdptr + pd_idx)->pd_base) << 12;
    XDEBUG_KPRINTF("[pagehandler] ptptr: %x\n", ptptr);

    return;
}
