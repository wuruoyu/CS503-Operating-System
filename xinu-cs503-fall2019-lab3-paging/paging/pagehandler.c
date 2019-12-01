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

frameid_t evict_frame() {
    // TODO
    XERROR_KPRINTF("[evict_frame] NOT IMPLEMENTED YET!\n");
    return SYSERR;
}

void pagehandler(void) {
    struct procent* prptr;
    pd_t* pdptr;
    pd_t* pdptr_ent;
    pt_t* ptptr;
    pt_t* ptptr_ent;

    frameid_t pt_fid;
    frameid_t pg_fid;

    char* faulted_addr;
    int faulted_addr_int;
    int faulted_addr_int_page;
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
    faulted_addr_int_page = (faulted_addr_int >> 12);
    offset = (faulted_addr_int & 0xFFF);
    XDEBUG_KPRINTF("[pagehandler] error code: %d, faulted_addr: %x, pd_idx: %d, pt_idx: %d, offset: %d, faulted_addr_int_page: %d\n", 
            error_code, faulted_addr, pd_idx, pt_idx, offset, faulted_addr_int_page);

    pdptr_ent = (pdptr + pd_idx);

    // pt not exist
    if (pdptr_ent->pd_avail == 0) {
        XDEBUG_KPRINTF("[pagehandler] pt not exist\n");

        // grab a frame
        pt_fid = find_free_frame();
        if (pt_fid == (frameid_t)SYSERR) {
            XDEBUG_KPRINTF("[pagehandler] Not available frame for pt\n");
            pt_fid = evict_frame();
        }
        XDEBUG_KPRINTF("[pagehandler] find available pt frame\n");

        // init the pt, pd_ent points to it
        init_pt(pt_fid);
        pdptr_ent->pd_avail = 1;
        pdptr_ent->pd_pres = 1;
        pdptr_ent->pd_base = ((int)(frameid_addr(pt_fid)) >> 12);
    }

    // we are not paging either pd or pt, means if pt exists, it always pres

    // stupid check
    if (pdptr_ent->pd_base == 0) {
        XDEBUG_KPRINTF("[pagehandler] should not equal to 0\n");
        return;
    }

    ptptr = ((pdptr + pd_idx)->pd_base) << 12;
    ptptr_ent = (ptptr + pt_idx);
    XDEBUG_KPRINTF("[pagehandler] ptptr: %x, ptptr_ent: %x\n", 
            ptptr, ptptr_ent);

    // if page does not exist
    if (!ptptr_ent->pt_avail) {
        XDEBUG_KPRINTF("[pagehandler] page not exist\n");
    
        // grab a frame
        pg_fid = find_free_frame();
        if (pg_fid == (frameid_t)SYSERR) {
            XDEBUG_KPRINTF("[pagehandler] Not available frame for page\n");
            pg_fid = evict_frame();
        }
        XDEBUG_KPRINTF("[pagehandler] find available page frame\n");

        // init the page, pt_ent points to it
        init_pg(pg_fid, faulted_addr_int_page);
        ptptr_ent->pt_avail = 1;
        ptptr_ent->pt_pres = 1;
        ptptr_ent->pt_base = ((int)(frameid_addr(pg_fid)) >> 12);
    } 

    // page exist but not pres
    if (!ptptr_ent->pt_pres) {
        // bring in the faulted page
    }

    return;
}