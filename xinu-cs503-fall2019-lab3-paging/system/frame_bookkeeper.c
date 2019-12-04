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

frameid_t gca_find_frame() {
    int         i;
    int         pid;
    pd_t*       pd_ptr; 
    pd_t*       pd_ent;
    pt_t*       pt_ent;
    pageid_t    vpage;
    int         pd_idx;
    int         pt_idx;

    // init
    for (i = 0; i < NFRAMES; i ++) {
        if (frame_bookkeeper[i].type != FRAME_PG) {
            continue;
        }

        // for each frame, find its pid, index into pt
        pid = frame_bookkeeper[i].pid;
        vpage = frame_bookkeeper[i].vpage;
        pd_ptr = proctab[pid].prpdptr;
        pd_idx = vpage >> 10;
        pt_idx = vpage & 0x3FF;
        pd_ent = (pd_ptr + pd_idx);
        pt_ent = (pt_t*)((pd_ent->pd_base) << 12) + pt_idx;

        // assign the corresponding value
        gca_keeper[i].reference = pt_ent->pt_acc;
        gca_keeper[i].modify = pt_ent->pt_dirty;
    }

    for (i = 0; i < NFRAMES; i ++) {
        if (frame_bookkeeper[i].type != FRAME_PG) {
            continue;
        }

        if (gca_keeper[i].reference == 0 && gca_keeper[i].modify == 0) {
            return i;
        }
        else if (gca_keeper[i].reference = 1 && gca_keeper[i].modify == 0) {
            gca_keeper[i].reference = 0;
        }
        else if (gca_keeper[i].reference = 1 && gca_keeper[i].modify == 1) {
            gca_keeper[i].modify = 0;
        }
    }

    for (i = 0; i < NFRAMES; i ++) {
        if (frame_bookkeeper[i].type != FRAME_PG) {
            continue;
        }

        if (gca_keeper[i].reference == 0 && gca_keeper[i].modify == 0) {
            return i;
        }
        else if (gca_keeper[i].reference = 1 && gca_keeper[i].modify == 0) {
            gca_keeper[i].reference = 0;
        }
        else if (gca_keeper[i].reference = 1 && gca_keeper[i].modify == 1) {
            gca_keeper[i].modify = 0;
        }
    }

    return SYSERR;
}
