#include <xinu.h>

syscall find_free_frame() {
    int i;

    for (i = 0; i < NFRAMES; i++) {
        if (frame_bookkeeper[i].state == FRAME_FREE) {
            return i;
        }
    }

    return SYSERR;
}

syscall bookkeep_frame_addr(char* addr, int type) {
    frameid_t fid = ADDR2FRAMEID(addr);
    if (frame_bookkeeper[fid].state == FRAME_OCCUPIED) {
       XERROR_KPRINTF("[init_pd_null] allocate wrong\n");
       return SYSERR;
    } 
    frame_bookkeeper[fid].state = FRAME_OCCUPIED;
    frame_bookkeeper[fid].type = FRAME_PD;
    frame_bookkeeper[fid].pid = currpid;
    XDEBUG_KPRINTF("frame %d (@%x) is allocated for %d by proc %d\n", 
            fid, addr, type, currpid);
    return OK;
}
