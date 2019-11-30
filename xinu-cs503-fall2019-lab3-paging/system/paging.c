
#include <xinu.h>

unsigned long tmp;

frameid_t addr_frameid(char* vaddr) {
    frameid_t fid = (((int)vaddr) >> 12) - 1024;
    if (fid < 0) {
        XERROR_KPRINTF("[addr_frameid] error\n");
    }
    return fid;
}

char* frameid_addr(frameid_t fid) {
    if (fid < 0) {
        XERROR_KPRINTF("[frameid_addr] error\n");
    }
    char* vaddr = (char*)((fid + 1024) << 12);
    return vaddr;
}

unsigned long read_cr3(void) {
    intmask mask;
    mask = disable();
    unsigned long local_tmp;

    asm("pushl %eax");
    asm("movl %cr3, %eax");
    asm("movl %eax, tmp");
    asm("popl %eax");

    local_tmp = tmp;

    restore(mask);

    return local_tmp;
}

void set_cr3(unsigned long n) {
    intmask mask;
    mask = disable();

    tmp = n;
    asm("pushl %eax");
    asm("movl tmp, %eax");		
    asm("movl %eax, %cr3");
    asm("popl %eax");

    restore(mask);
    return;
}

unsigned long read_cr0(void) {
    intmask mask;
    mask = disable();
    unsigned long local_tmp;

    asm("pushl %eax");
    asm("movl %cr0, %eax");
    asm("movl %eax, tmp");
    asm("popl %eax");

    local_tmp = tmp;

    restore(mask);

    return local_tmp;
}

void set_cr0(unsigned long n) {
    intmask mask;
    mask = disable();

    tmp = n;
    asm("pushl %eax");
    asm("movl tmp, %eax");		
    asm("movl %eax, %cr0");
    asm("popl %eax");

    restore(mask);
    return;
}

void init_pd(frameid_t fid) {
    int i;
    pd_t* pd_ptr = (pd_t*)(frameid_addr(fid));

    bookkeep_frame_addr(pd_ptr, FRAME_PD);

    for (i = 0; i < PAGEDIRSIZE; i ++) {
        (pd_ptr + i)->pd_pres = 0;
        (pd_ptr + i)->pd_write = 1;
        (pd_ptr + i)->pd_user = 0;
        (pd_ptr + i)->pd_pwt = 0;
        (pd_ptr + i)->pd_pcd = 0;
        (pd_ptr + i)->pd_acc = 0;
        (pd_ptr + i)->pd_mbz = 0;
        (pd_ptr + i)->pd_fmb = 0;
        (pd_ptr + i)->pd_global = 0;
        (pd_ptr + i)->pd_avail = 0;
        (pd_ptr + i)->pd_base = 0;
    }
}

void init_pd_null(pd_t* pd_ptr) {
    int i;
    pt_t* pt_ptr = (pt_t*)(pd_ptr + PAGEDIRSIZE);

    // bookkeep for pd
    bookkeep_frame_addr(pd_ptr, FRAME_PD);

    for (i = 0; i < PAGEDIRSIZE; i ++) {

        // first 4096 frames: identity map
        if (i < 4) {
            (pd_ptr + i)->pd_pres = 1;
            (pd_ptr + i)->pd_write = 1;
            (pd_ptr + i)->pd_user = 0;
            (pd_ptr + i)->pd_pwt = 0;
            (pd_ptr + i)->pd_pcd = 0;
            (pd_ptr + i)->pd_acc = 0;
            (pd_ptr + i)->pd_mbz = 0;
            (pd_ptr + i)->pd_fmb = 0;
            (pd_ptr + i)->pd_global = 0;
            (pd_ptr + i)->pd_avail = 0;
            (pd_ptr + i)->pd_base = ((int)pt_ptr >> 12);  

            init_pt_null(i, pt_ptr);

            pt_ptr += PAGETABSIZE;
        }
        // device space
        else if (i == 576) {
            (pd_ptr + i)->pd_pres = 1;
            (pd_ptr + i)->pd_write = 1;
            (pd_ptr + i)->pd_user = 0;
            (pd_ptr + i)->pd_pwt = 0;
            (pd_ptr + i)->pd_pcd = 0;
            (pd_ptr + i)->pd_acc = 0;
            (pd_ptr + i)->pd_mbz = 0;
            (pd_ptr + i)->pd_fmb = 0;
            (pd_ptr + i)->pd_global = 0;
            (pd_ptr + i)->pd_avail = 0;
            (pd_ptr + i)->pd_base = ((int)pt_ptr >> 12);  

            init_pt_null(i, pt_ptr);

            pt_ptr += PAGETABSIZE;
        }
        // unmapped addr
        else {
            (pd_ptr + i)->pd_pres = 0;
            (pd_ptr + i)->pd_write = 0;
            (pd_ptr + i)->pd_user = 0;
            (pd_ptr + i)->pd_pwt = 0;
            (pd_ptr + i)->pd_pcd = 0;
            (pd_ptr + i)->pd_acc = 0;
            (pd_ptr + i)->pd_mbz = 0;
            (pd_ptr + i)->pd_fmb = 0;
            (pd_ptr + i)->pd_global = 0;
            (pd_ptr + i)->pd_avail = 0;
            (pd_ptr + i)->pd_base = 0;
        }
         
        /*XDEBUG_KPRINTF("[init_pd_null] pd ent: %d, pd_ptr: %x, pd_base: %x\n", */
                /*i, (pd_ptr+i), (pd_ptr+i)->pd_base);*/
    }
}

void init_pt_null(int pd_idx, pt_t* pt_ptr) {
    int i;

    /*XDEBUG_KPRINTF("[init_pt_null] pd_idx: %d, pt_ptr: %x\n", */
            /*pd_idx, pt_ptr);*/

    // bookkeep for pt
    bookkeep_frame_addr(pt_ptr, FRAME_PT);

    for (i = 0; i < PAGETABSIZE; i ++) {
        (pt_ptr+i)->pt_pres = 1;
        (pt_ptr+i)->pt_write = 1;
        (pt_ptr+i)->pt_user = 0;
        (pt_ptr+i)->pt_pwt = 0;
        (pt_ptr+i)->pt_pcd = 0;
        (pt_ptr+i)->pt_acc = 0;
        (pt_ptr+i)->pt_dirty = 0;
        (pt_ptr+i)->pt_mbz = 0;
        (pt_ptr+i)->pt_global = 0;
        (pt_ptr+i)->pt_avail = 0;
        (pt_ptr+i)->pt_base = (PAGETABSIZE) * pd_idx + i;

        /*XDEBUG_KPRINTF("[init_pt_null] pd_idx: %d, pt_idx: %d, pt_base: %x\n", */
            /*pd_idx, i, pt_ptr->pt_base);*/
    }
}

void enable_paging() {
    unsigned long local_tmp = read_cr0();
    /*XDEBUG_KPRINTF("[enable_paging] before set cr0: %x\n", local_tmp);*/
    local_tmp = local_tmp | (0x80000000);
    /*XDEBUG_KPRINTF("[enable_paging] after set cr0: %x\n", local_tmp);*/
    set_cr0(local_tmp);
}

void test() {

}

/* init PT and PE for the NULL proc */
syscall initialize_paging_null() {

    // test
    test();

    // init PD and PT
    struct pd_t* pd_ptr;
    pd_ptr = (pd_t*)METADATA_PHY_MIN;
    init_pd_null(pd_ptr);

    proctab[NULLPROC].prpdptr = pd_ptr;
    
    // set PDBR reg to the PD of NULL proc
    set_cr3((unsigned long)proctab[NULLPROC].prpdptr);
    /*XDEBUG_KPRINTF("[set_cr3] after set cr3: %x\n", read_cr3());*/
    
    // register page fault ISR
    set_evec(14, (uint32)pagedisp);
    
    // enable paging
    enable_paging();
}
