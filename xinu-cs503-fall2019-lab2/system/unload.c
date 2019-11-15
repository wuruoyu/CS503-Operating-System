/* unload.c - unload elf */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  unload - unload elf 
 *------------------------------------------------------------------------
 */


int unload(void* ld_text_addr) {

	intmask mask;
	mask = disable();
	int i;
    int j;

	for (i = 0; i < NLOAD; i ++) {
		if (loadtab[i].status == LOAD_OCCUPIED) {
			if (loadtab[i].ld_text_addr == ld_text_addr) {
                if (loadtab[i].automatic_load_state == AUTOMATIC_ON) {
                    loadtab[i].status = LOAD_FREE;
                    restore(mask);
                    return OK;
                }
				loadtab[i].status = LOAD_FREE;
				freebuf(loadtab[i].exec);
				XDEBUG_KPRINTF("[unloader] Gocha and Free!\n");
    
                // check automatic loading
                for (j = 0; j < NLOAD; j ++) {
                    if (loadtab[j].automatic_load_state == AUTOMATIC_ON 
                            && loadtab[j].load_by == i 
                            && loadtab[j].status == LOAD_FREE) {

                        freebuf(loadtab[j].exec);
                        loadtab[j].automatic_load_state = AUTOMATIC_OFF;
                    }
                }

	            restore(mask);
                return OK;
			}
		}
	}

    XDEBUG_KPRINTF("[unload] not find\n");
	restore(mask);
    return SYSERR;
}
