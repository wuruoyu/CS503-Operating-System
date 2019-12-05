#include <xinu.h>

extern int error_code;
unsigned long temp;
int           evict_tmp;

unsigned long read_cr2(void) {
    intmask mask;
    mask = disable();
    unsigned long local_tmp;

    asm("pushl %eax");
    asm("movl %cr2, %eax");
    asm("movl %eax, temp");
    asm("popl %eax");

    local_tmp = temp;

    restore(mask);

    return local_tmp;
}


frameid_t evict_frame() {
    frameid_t   fid;
    pid32       pid; 
    pageid_t    vpage;
    int         pd_idx;
    int         pt_idx;
    pd_t*       pdptr_ent;
    pt_t*       ptptr_ent;
    frameid_t   pt_frame_id;

    int         i;
    char*       pg_addr;
    int         bs_id;
    int         bs_page_offset;

    intmask     mask;

    mask = disable();

    if (currpolicy == FIFO) {
        fid = fifo_find_frame();
    }
    else if (currpolicy == GCA) {
        fid = gca_find_frame();
    }
    else {
        XERROR_KPRINTF("[evict_frame] policy wrong\n");
    }

    if (fid == SYSERR) {
        XERROR_KPRINTF("error in page replacement policy\n");
    }

    pid = frame_bookkeeper[fid].pid;
    vpage = frame_bookkeeper[fid].vpage;
    pg_addr = frameid_addr(fid);

    pd_idx = vpage >> 10;
    pt_idx = vpage & 0x3FF;

    XDEBUG_KPRINTF("[evict_frame] fid: %d, pid: %d, pd_idx: %d, pt_idx: %d, vpage: %d\n",
            fid, pid, pd_idx, pt_idx, vpage);

    pdptr_ent = (proctab[pid].prpdptr + pd_idx);
    ptptr_ent = (pt_t*)((pdptr_ent->pd_base) << 12) + pt_idx;

    XDEBUG_KPRINTF("[evict_frame] pdptr_ent: %x, pdptr_ent: %x\n", pdptr_ent, ptptr_ent);

    // mark as not present
    ptptr_ent->pt_pres = 0;

    // current process's page
    if (currpid == pid) {
        evict_tmp = (proctab[pid].prpdptr);
        asm("pushl %eax");
        asm("invlpg evict_tmp");
        asm("popl %eax");

        evict_tmp = (vpage << 12);
        asm("pushl %eax");
        asm("invlpg evict_tmp");
        asm("popl %eax");
        XDEBUG_KPRINTF("[evict_frame] evict_frame is from current process\n");
    }

    /*decrement pt's counting. if zero, mark not pres*/
    pt_frame_id = addr_frameid((pdptr_ent->pd_base) << 12);
    frame_bookkeeper[pt_frame_id].count --;
    if (frame_bookkeeper[pt_frame_id].count == 0) {
        XDEBUG_KPRINTF("[evict_frame] pt no need\n");
        pdptr_ent->pd_pres = 0;
        // un bookkeep it
        frame_bookkeeper[pt_frame_id].state = FRAME_FREE;
    }
    
    // dirty bit
    if (ptptr_ent->pt_dirty) {

        // find corresponding bs
        for (i = 0; i < MAX_BS_ENTRIES; i ++) {
            if (bstab[i].isallocated == TRUE && bstab[i].pid == currpid) {
                if (vpage >= bstab[i].vpage && vpage < bstab[i].vpage + bstab[i].npages) {
                    bs_id = i;
                    bs_page_offset = vpage - bstab[i].vpage;

                    XDEBUG_KPRINTF("[evict_frame] bs_id: %d, pid: %d, offset: %d\n",
                            bs_id, pid, bs_page_offset);

                    break;
                }
            }
            if (i == MAX_BS_ENTRIES - 1) {
                XERROR_KPRINTF("[evict_frame] no found in bs stote\n");
                kill(pid);
                restore(mask);
                return SYSERR;
            }
        }
        

        XDEBUG_KPRINTF("[evict_frame] evict dirty\n");

        while (SYSERR == (syscall)write_bs(pg_addr, bs_id, bs_page_offset)) {
            XERROR_KPRINTF("[evict_frame] write_bs fail\n");
        }
        XDEBUG_KPRINTF("[evict_frame] finish writing to bs\n");
    }
    else {
        XDEBUG_KPRINTF("[evict_frame] Not dirty\n");
    }
    
    // mark the frame as FREE in bookkeeper
    /*bookkeep_frame_reset(fid);*/
    frame_bookkeeper[fid].state = FRAME_FREE;
    /*XERROR_KPRINTF("[evict_frame] we are here\n");*/

    hook_pswap_out(pid, vpage, fid);

    restore(mask);
    return fid;
}

void pagehandler(void) {
    intmask mask;

    mask = disable();

    // lab3 hook
    npagefault ++;
    XDEBUG_KPRINTF("[[pagehandler] npgfault: %d\n", npagefault);

    int i;

    struct procent* prptr;
    pd_t* pdptr;
    pd_t* pdptr_ent;
    pt_t* ptptr;
    pt_t* ptptr_ent;
    frameid_t pt_frame_id;

    frameid_t pt_fid;
    frameid_t pg_fid;
    char*     pg_addr;

    char* faulted_addr;
    int faulted_addr_int;
    int faulted_addr_page;
    int pd_idx;
    int pt_idx;
    int offset;

    int bs_page_offset;
    int bs_id;

    faulted_addr = read_cr2();
    faulted_addr_int = (int)faulted_addr;

    prptr = &proctab[currpid];
    pdptr = prptr->prpdptr;

    // check if faulted_addr valid
    if (faulted_addr_int < 0x1000000 
            || faulted_addr_int > (0x1000000 + (prptr->vsize) * NBPG)) {
        XDEBUG_KPRINTF("[pagehandler] faulted_addr error\n");
        kill(currpid);
        restore(mask);
        return;
    }

    pd_idx = (faulted_addr_int >> 22);
    pt_idx = ((faulted_addr_int & 0x3FF000) >> 12);
    faulted_addr_page = (faulted_addr_int >> 12);
    offset = (faulted_addr_int & 0xFFF);
    XDEBUG_KPRINTF("[pagehandler] error code: %d, faulted_addr: %x, pd_idx: %d, pt_idx: %d, offset: %d, faulted_addr_page: %d\n", 
            error_code, faulted_addr, pd_idx, pt_idx, offset, faulted_addr_page);

    pdptr_ent = (pdptr + pd_idx);

    // pt not exist
    if (pdptr_ent->pd_avail == 0) {
        XDEBUG_KPRINTF("[pagehandler] pt not exist\n");

        // grab a frame
        pt_fid = find_free_frame();
        if (pt_fid == (frameid_t)SYSERR) {
            XDEBUG_KPRINTF("[pagehandler] Not available frame for pt\n");
            pt_fid = evict_frame();
            if ((syscall)pt_fid == SYSERR) {
                XERROR_KPRINTF("[pagehandler] evict_frame wrong\n");
                restore(mask);
                return;
            }
        }
        XDEBUG_KPRINTF("[pagehandler] find available pt frame\n");

        // init the pt, pd_ent points to it
        init_pt(pt_fid); 
        pdptr_ent->pd_avail = 1;
        pdptr_ent->pd_pres = 1;
        pdptr_ent->pd_base = ((int)(frameid_addr(pt_fid)) >> 12);

        XDEBUG_KPRINTF("[pagehandler] init the pt\n");
    }

    // we are not paging either pd or pt, means if pt exists, it always pres

    // stupid check
    if (pdptr_ent->pd_base == 0) {
        XERROR_KPRINTF("[pagehandler] should not equal to 0\n");
        restore(mask);
        return;
    }

    ptptr = ((pdptr + pd_idx)->pd_base) << 12;
    ptptr_ent = (ptptr + pt_idx);
    pt_frame_id = addr_frameid(ptptr);
    XDEBUG_KPRINTF("[pagehandler] ptptr: %x, ptptr_ent: %x, pt_frame_id: %d\n", 
            ptptr, ptptr_ent, pt_frame_id);

    // if page does not exist
    if (!ptptr_ent->pt_avail) {
        XDEBUG_KPRINTF("[pagehandler] page not exist\n");
    
        // grab a frame
        pg_fid = find_free_frame();
        if (pg_fid == (frameid_t)SYSERR) {
            XDEBUG_KPRINTF("[pagehandler] Not available frame for page\n");
            pg_fid = evict_frame();
            if ((syscall)pt_fid == SYSERR) {
                XERROR_KPRINTF("[pagehandler] evict_frame wrong\n");
                restore(mask);
                return;
            }
        }
        XDEBUG_KPRINTF("[pagehandler] find available page frame %d\n", pg_fid);

        // init the page, pt_ent points to it
        init_pg(pg_fid, faulted_addr_page);
        ptptr_ent->pt_avail = 1;
        ptptr_ent->pt_pres = 1;
        ptptr_ent->pt_base = ((int)(frameid_addr(pg_fid)) >> 12);

        // increment the ref count
        frame_bookkeeper[pt_frame_id].count ++;

        /*XERROR_KPRINTF("[pagehandler] init the page\n");*/
    } 
    // page exist but not pres
    else if (!ptptr_ent->pt_pres) {
        XDEBUG_KPRINTF("[pagehandler] page not exist\n");

        // it should be in the bstab
        for (i = 0; i < MAX_BS_ENTRIES; i ++) {
            if (bstab[i].isallocated == TRUE && bstab[i].pid == currpid) {
                if (faulted_addr_page >= bstab[i].vpage && 
                        faulted_addr_page < bstab[i].vpage + bstab[i].npages) {
                    bs_page_offset = faulted_addr_page - bstab[i].vpage;
                    bs_id = i;

                    XDEBUG_KPRINTF("[pagehandler: page exist not pres] bs idx: %d, pid: %d, vpage: %d, npages: %d, offset: %d\n",
                            i, bstab[i].pid, bstab[i].vpage, bstab[i].npages, bs_page_offset);

                    break;
                }
            }
            if (i == MAX_BS_ENTRIES - 1) {
                XERROR_KPRINTF("[pagehandler: page exist not pres] not found in bs store\n");
                restore(mask);
                return;
            }
        }

        // increment the ref count
        frame_bookkeeper[pt_frame_id].count ++;

        // grab a frame
        pg_fid = find_free_frame();
        /*XERROR_KPRINTF("[pagehandler] page pg_fid: %d\n", pg_fid);*/
        if (pg_fid == (frameid_t)SYSERR) {
            XDEBUG_KPRINTF("[pagehandler] Not available frame for page\n");
            pg_fid = evict_frame();
            if ((syscall)pt_fid == SYSERR) {
                XERROR_KPRINTF("[pagehandler] evict_frame wrong\n");
                restore(mask);
                return;
            }
            /*XERROR_KPRINTF("[pagehandler] after evict_frame pg_fid: %d\n", pg_fid);*/
        }
        XERROR_KPRINTF("[pagehandler] find available page frame: %d\n", pg_fid);
        pg_addr = frameid_addr(pg_fid);

        // read from bs
        XDEBUG_KPRINTF("[pagehandler] read from bs, addr: %x, id: %d, offset: %d\n", 
                pg_addr, bs_id, bs_page_offset);
        while (SYSERR == (syscall)read_bs(pg_addr, bs_id, bs_page_offset)) {
            XERROR_KPRINTF("[pagehandler] read_bs fail\n");
        }
        XDEBUG_KPRINTF("[pagehandler] finish reading from bs\n");

        // update field
        bookkeep_frame_id(pg_fid, FRAME_PG, faulted_addr_page);
        ptptr_ent->pt_pres = 1;
        ptptr_ent->pt_base = ((int)pg_addr >> 12);
    }
    else {
        XERROR_KPRINTF("[pagehandler] should not have error\n");
        restore(mask);
        return;
    }

    hook_pfault(currpid, faulted_addr, faulted_addr_page, pg_fid);
    /*XTEST_KPRINTF("[pagehandler] currpid: %d\n", currpid);*/

    restore(mask);
    return;
}
