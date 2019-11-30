#include <xinu.h>

syscall find_free_frame() {
    int i;

    for (i = 0; i < NFRAMES; i++) {
        if (frame_bookkeeper[i].state == FRAME_FREE) {
            return i;
        }
    }

    XERROR_KPRINTF("[find_free_frame] no available frame\n");
    return SYSERR;
}

syscall bookkeep_frame_addr(char* addr, int type) {
    frameid_t fid = addr_frameid(addr);
    if (frame_bookkeeper[fid].state == FRAME_OCCUPIED) {
       XERROR_KPRINTF("[init_pd_null] allocate wrong\n");
       return SYSERR;
    } 
    frame_bookkeeper[fid].state = FRAME_OCCUPIED;
    frame_bookkeeper[fid].type = FRAME_PD;
    frame_bookkeeper[fid].pid = currpid;
    XDEBUG_KPRINTF("[frame_bookkeeper] frame %d (@%x) is allocated for %d by proc %d\n", 
            fid, addr, type, currpid);
    return OK;
}

syscall bookkeep_frame_id(frameid_t fid, int type) {
    if (frame_bookkeeper[fid].state == FRAME_OCCUPIED) {
       XERROR_KPRINTF("[init_pd_null] allocate wrong\n");
       return SYSERR;
    } 
    frame_bookkeeper[fid].state = FRAME_OCCUPIED;
    frame_bookkeeper[fid].type = FRAME_PD;
    frame_bookkeeper[fid].pid = currpid;
    XDEBUG_KPRINTF("[frame_bookkeeper] frame %d is allocated for %d by proc %d\n", 
            fid, type, currpid);
    return OK;
}

/* 1. check if there is enough frames in the mem
 *  1.1 evict frames to bs
 *  1.2 update structure
 * 2. update pd and pt
 * 3. package it as memblk
 */


syscall bookkeep_get_frame(int nframe) {
    int mem_nframe = 0;
    int i;
    
    // check if there is enough frames in the mem
    for (i = 0; i < NFRAMES; i ++) {
        if(frame_bookkeeper[i].state == FRAME_FREE) {
            mem_nframe ++;
        }
    }
    XDEBUG_KPRINTF("[bookkeep_get_frame] available frame: %d\n", mem_nframe);
    
    // we have enough
    if (nframe <= mem_nframe) {
        for (i = 0; i < NFRAMES; i ++) {
            if(frame_bookkeeper[i].state == FRAME_FREE) {

            }
        }
    }
    // we do eviction
    else {
    }
}
