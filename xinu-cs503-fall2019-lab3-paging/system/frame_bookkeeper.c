#include <xinu.h>

#define INT_MAX 2147483647

syscall find_free_frame() {
    int i;

    for (i = 0; i < NFRAMES; i++) {
        if (frame_bookkeeper[i].state == FRAME_FREE) {
            return i;
        }
    }

    XDEBUG_KPRINTF("[find_free_frame] no available frame in mem\n");
    return SYSERR;
}

syscall bookkeep_frame_addr(char* addr, int frame_type, pageid_t vpage_id) {
    frameid_t fid = addr_frameid(addr);
    if (frame_bookkeeper[fid].state == FRAME_OCCUPIED) {
       XERROR_KPRINTF("[bookkeep_frame_addr] allocate wrong\n");
       return SYSERR;
    } 
    frame_bookkeeper[fid].state = FRAME_OCCUPIED;
    frame_bookkeeper[fid].type = frame_type;
    frame_bookkeeper[fid].pid = currpid;
    /*frame_bookkeeper[fid].vpage = 0;*/
    /*frame_bookkeeper[fid].count = 0;*/
    /*frame_bookkeeper[fid].fifo_tag = 0;*/

    if (frame_type == FRAME_PG) {
        frame_bookkeeper[fid].vpage = vpage_id;
        frame_bookkeeper[fid].fifo_tag = time_tag;
        time_tag ++;
    }

    XDEBUG_KPRINTF("[frame_bookkeeper] frame %d (@%x) is allocated for %d by proc %d\n", 
            fid, addr, frame_type, currpid);
    return OK;
}

syscall bookkeep_frame_id(frameid_t fid, int frame_type, pageid_t vpage_id) {
    if (frame_bookkeeper[fid].state == FRAME_OCCUPIED) {
       XERROR_KPRINTF("[bookkeep_frame_id] allocate wrong\n");
       return SYSERR;
    } 
    frame_bookkeeper[fid].state = FRAME_OCCUPIED;
    frame_bookkeeper[fid].type = frame_type;
    frame_bookkeeper[fid].pid = currpid;
    /*frame_bookkeeper[fid].vpage = 0;*/
    /*frame_bookkeeper[fid].count = 0;*/
    /*frame_bookkeeper[fid].fifo_tag = 0;*/

    if (frame_type == FRAME_PG) {
        frame_bookkeeper[fid].vpage = vpage_id;
        frame_bookkeeper[fid].fifo_tag = time_tag;
        time_tag ++;
    }

    XDEBUG_KPRINTF("[frame_bookkeeper] frame %d is allocated for %d by proc %d\n", 
            fid, frame_type, currpid);
    return OK;
}

syscall bookkeep_frame_reset(frameid_t fid) {
    frame_bookkeeper[fid].state = FRAME_FREE;
    frame_bookkeeper[fid].pid   = 0;
    frame_bookkeeper[fid].vpage = 0;
    XDEBUG_KPRINTF("[bookkeep_frame_reset] reset %d\n, fid");
    return OK;
}


frameid_t fifo_find_frame() {
    int i;

    int min_id = -1;
    int min_tag = INT_MAX;

    for (i = 0; i < NFRAMES; i++) {
        if (frame_bookkeeper[i].state == FRAME_FREE) {
            XERROR_KPRINTF("[fifo_find_frame] There is free frame\n");
            return SYSERR;
        }
        if (frame_bookkeeper[i].type != FRAME_PG) {
            continue;
        }
        if (frame_bookkeeper[i].fifo_tag < min_tag) {
            min_id = i;
            min_tag = frame_bookkeeper[i].fifo_tag;
        }
    }

    XDEBUG_KPRINTF("[fifo_find_frame] fid: %d, tag: %d\n", min_id, min_tag);

    if (frame_bookkeeper[min_id].type != FRAME_PG || min_id == -1) {
        XERROR_KPRINTF("[fifo_find_frame] wrong!\n");
    }

    return min_id;
}
