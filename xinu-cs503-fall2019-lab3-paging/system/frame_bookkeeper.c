#include <xinu.h>

syscall find_free_frame() {
    int i;

    for (i = 0; i < NFRAMES; i++) {
        if (frame_bookkeeper[i].state == FRAME_FREE) {
            return i;
        }
    }

    XERROR_KPRINTF("[find_free_frame] no available frame in mem\n");
    return SYSERR;
}

syscall bookkeep_frame_addr(char* addr, int frame_type, pageid_t vpage_id) {
    frameid_t fid = addr_frameid(addr);
    if (frame_bookkeeper[fid].state == FRAME_OCCUPIED) {
       XERROR_KPRINTF("[init_pd_null] allocate wrong\n");
       return SYSERR;
    } 
    frame_bookkeeper[fid].state = FRAME_OCCUPIED;
    frame_bookkeeper[fid].type = frame_type;
    frame_bookkeeper[fid].pid = currpid;

    if (frame_type == FRAME_PG) {
        frame_bookkeeper[fid].vpage = vpage_id;
    }

    XDEBUG_KPRINTF("[frame_bookkeeper] frame %d (@%x) is allocated for %d by proc %d\n", 
            fid, addr, frame_type, currpid);
    return OK;
}

syscall bookkeep_frame_id(frameid_t fid, int frame_type, pageid_t vpage_id) {
    if (frame_bookkeeper[fid].state == FRAME_OCCUPIED) {
       XERROR_KPRINTF("[init_pd_null] allocate wrong\n");
       return SYSERR;
    } 
    frame_bookkeeper[fid].state = FRAME_OCCUPIED;
    frame_bookkeeper[fid].type = frame_type;
    frame_bookkeeper[fid].pid = currpid;

    if (frame_type == FRAME_PG) {
        frame_bookkeeper[fid].vpage = vpage_id;
    }

    XDEBUG_KPRINTF("[frame_bookkeeper] frame %d is allocated for %d by proc %d\n", 
            fid, frame_type, currpid);
    return OK;
}


