#include <xinu.h>

// TODO: no need to have interrupt disabled or semaphore ?
syscall bs_map_get(pid32 pid, pageid_t vpage, bsd_t* bs_id, int* page_offset) {
    int i;

    for (i = 0; i < NBSMAP; i ++) {
        if (bs_map[i].state == BSMAP_OCCUPIED && bs_map[i].pid == pid) {
            if (vpage < bs_map[i].vpage || vpage >= bs_map[i].vpage + bs_map[i].npage) {
                continue;
            }
            *bs_id = bs_map[i].bs_id;
            *page_offset = vpage - bs_map[i].vpage;
            return OK;
        }
    }

    XDEBUG_KPRINTF("[bs_map_get] \n");
    return SYSERR;
}
