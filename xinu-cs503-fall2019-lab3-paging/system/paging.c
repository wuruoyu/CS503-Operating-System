#include <xinu.h>

char* PT_ADDR[4];
char* DEVICE_PT_ADDR;

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

void init_pd(frameid_t fid) {
    int i;
    intmask mask;
    pd_t* pd_ptr = (pd_t*)(frameid_addr(fid));

    mask = disable();

    bookkeep_frame_addr(pd_ptr, FRAME_PD, 0);

    for (i = 0; i < PAGEDIRSIZE; i ++) {
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
            (pd_ptr + i)->pd_base = PT_ADDR[i];

            XDEBUG_KPRINTF("[init_pd] %x base: %x\n", (pd_ptr + i), (pd_ptr + i)->pd_base);
        }
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
            (pd_ptr + i)->pd_base = DEVICE_PT_ADDR;

            XDEBUG_KPRINTF("[init_pd] %x base: %x\n", (pd_ptr + i), (pd_ptr + i)->pd_base);
        }
        else {
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

    /*XERROR_KPRINTF("[init_pd] one more pd\n");*/

    restore(mask);
}

void init_pt(frameid_t fid) {
    int i;
    pt_t* pt_ptr;
    intmask mask;

    mask = disable();

    bookkeep_frame_id(fid, FRAME_PT, 0);
    pt_ptr = (pt_t*)frameid_addr(fid);

    XDEBUG_KPRINTF("[init_pt] fid: %d, pt_ptr: %x\n", 
            fid, pt_ptr);

    for (i = 0; i < PAGETABSIZE; i ++) {
        (pt_ptr+i)->pt_pres = 0;
        (pt_ptr+i)->pt_write = 1;
        (pt_ptr+i)->pt_user = 0;
        (pt_ptr+i)->pt_pwt = 0;
        (pt_ptr+i)->pt_pcd = 0;
        (pt_ptr+i)->pt_acc = 0;
        (pt_ptr+i)->pt_dirty = 0;
        (pt_ptr+i)->pt_mbz = 0;
        (pt_ptr+i)->pt_global = 0;
        (pt_ptr+i)->pt_avail = 0;
        (pt_ptr+i)->pt_base = 0;
    }

    // lab3 hook
    hook_ptable_create(fid + 1024);

    restore(mask);
}

void init_pg(frameid_t fid, pageid_t vpage_id) {
    intmask mask;

    mask = disable();

    bookkeep_frame_id(fid, FRAME_PG, vpage_id);

    restore(mask);
}

void init_pd_null(pd_t* pd_ptr) {
    int i;
    intmask mask;
    pt_t* pt_ptr = (pt_t*)(pd_ptr + PAGEDIRSIZE);

    mask = disable();

    // bookkeep for pd
    bookkeep_frame_addr(pd_ptr, FRAME_PD, 0);

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

            PT_ADDR[i] = ((int)pt_ptr >> 12);
            XDEBUG_KPRINTF("[init_pd_null] PT_ADDR[%d]: %x\n", i, PT_ADDR[i]);

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

            DEVICE_PT_ADDR = ((int)pt_ptr >> 12);
            XDEBUG_KPRINTF("[init_pd_null] DEVICE_PT_ADDR[%d]: %x\n", i, DEVICE_PT_ADDR);

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

    restore(mask);
}

void init_pt_null(int pd_idx, pt_t* pt_ptr) {
    int i;
    intmask mask;
    
    mask = disable();

    /*XDEBUG_KPRINTF("[init_pt_null] pd_idx: %d, pt_ptr: %x\n", */
            /*pd_idx, pt_ptr);*/

    // bookkeep for pt
    bookkeep_frame_addr(pt_ptr, FRAME_PT, 0);

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

    // lab3 hook
    hook_ptable_create(addr_frameid(pt_ptr) + 1024);

    restore(mask);
}

void enable_paging() {
    intmask mask;

    mask = disable();

    unsigned long local_tmp = read_cr0();
    /*XDEBUG_KPRINTF("[enable_paging] before set cr0: %x\n", local_tmp);*/
    local_tmp = local_tmp | (0x80000000);
    /*XDEBUG_KPRINTF("[enable_paging] after set cr0: %x\n", local_tmp);*/
    set_cr0(local_tmp);

    restore(mask);
}


/* init PT and PE for the NULL proc */
syscall initialize_paging_null() {

    intmask mask;

    mask = disable();

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

    restore(mask);
}


