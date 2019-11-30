#include <xinu.h>

/*syscall bstab_get(pid32 pid, char* vaddr, bsd_t* bs_id, int* page_offset_within_store) {*/
    /*int i;*/

    /*frameid_t fid = addr2frameid(vaddr);*/
    /*for (i = 0; i < MAX_BS_ENTRIES; i ++) {*/
        /*if (bstab[i].isallocated == TRUE && bstab[i].pid == pid) {*/
            /*if (fid < )*/
        /*}*/
    /*}*/

    /*XDEBUG_KPRINTF("[bs_map_get] \n");*/
    /*return SYSERR;*/
/*}*/

/*syscall bs_map_put(pid32 pid, pageid_t vpage, bsd_t* bs_id, int* page_offset) {*/
    /*int i;*/

    /*for (i = 0; i < NBSMAP; i ++) {*/
        /*if (bs_map[i].state == BSMAP_OCCUPIED && bs_map[i].pid == pid) {*/
            /*if (vpage < bs_map[i].vpage || vpage >= bs_map[i].vpage + bs_map[i].npage) {*/
                /*continue;*/
            /*}*/
            /**bs_id = bs_map[i].bs_id;*/
            /**page_offset = vpage - bs_map[i].vpage;*/
            /*return OK;*/
        /*}*/
    /*}*/

    /*XDEBUG_KPRINTF("[bs_map_get] \n");*/
    /*return SYSERR;*/
/*}*/

int bstab_get_remaining_page() {
    int i;

    int n_unallocated = 0;
    for (i = 0; i < MAX_BS_ENTRIES; i ++) {
        if (bstab[i].isallocated == FALSE) {
            n_unallocated ++;
        }
    }
    return n_unallocated * MAX_PAGES_PER_BS;
}
