#include <xinu.h>

// TODO: no need to have interrupt disabled or semaphore ?

syscall inv_pt_get(frameid_t frame_id, pid32* pid, pageid_t* vpage_id) {
    
    int i;

    if (frame_id >= NFRAMES) {
        XDEBUG_KPRINTF("[inv_pt] frame_id\n");
        return SYSERR;
    }

    if (inv_pt[frame_id].state != INVPT_OCCUPIED) {
        XDEBUG_KPRINTF("[inv_pt] frame_id\n");
        return SYSERR;
    }

    *pid = inv_pt[frame_id].pid;
    *vpage_id =  inv_pt[frame_id].vpage_id;
    return OK;
}

syscall inv_pt_put(frameid_t frame_id, pid32 pid, pageid_t vpage_id) {
    int i;

    if (frame_id >= NFRAMES) {
        XDEBUG_KPRINTF("[inv_pt] frame_id\n");
        return SYSERR;
    }

    if (inv_pt[frame_id].state == INVPT_OCCUPIED) {
        XDEBUG_KPRINTF("[inv_pt]\n");
        return SYSERR;
    }

    inv_pt[frame_id].state = INVPT_OCCUPIED;
    inv_pt[frame_id].pid = pid;
    inv_pt[frame_id].vpage_id = vpage_id;
    return OK;
}
