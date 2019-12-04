/* kill.c - kill */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  kill  -  Kill a process and remove it from the system
 *------------------------------------------------------------------------
 */
syscall	kill(
	  pid32		pid		/* ID of process to kill	*/
	)
{
	intmask	mask;			/* Saved interrupt mask		*/
	struct	procent *prptr;		/* Ptr to process' table entry	*/
	int32	i;			/* Index into descriptors	*/

    int ret;
    pd_t*   pd_ptr;
    pt_t*   pt_ptr;

	mask = disable();
	if (isbadpid(pid) || (pid == NULLPROC)
	    || ((prptr = &proctab[pid])->prstate) == PR_FREE) {
		restore(mask);
		return SYSERR;
	}

	//kprintf("KILL : %d\n", pid);
	if (--prcount <= 1) {		/* Last user process completes	*/
		xdone();
	}
	
	send(prptr->prparent, pid);
	for (i=0; i<3; i++) {
		close(prptr->prdesc[i]);
	}

    /* Lab3 TODO. Free frames as a process gets killed. */

    // deallocate bs & bstab release
    for (i = 0; i < MAX_BS_ENTRIES; i ++) {
        if (bstab[i].isallocated == TRUE && bstab[i].pid == pid) {
            ret = deallocate_bs(i);
            if (ret == (syscall)SYSERR) {
                XERROR_KPRINTF("[kill] error\n");
            }
        }
    }

    // release the pd, except the null's
    pd_ptr = proctab[pid].prpdptr;
    if (addr_frameid(pd_ptr) != 0) {
        frame_bookkeeper[addr_frameid(pd_ptr)].state = FRAME_FREE; 
        // release the pt 
        for (i = 0; i < PAGEDIRSIZE; i ++) {
            // dont release the null's
            if (i < 4 || i == 576) {
                continue;
            }

            if ((pd_ptr + i)->pd_pres == 1) {
                (pd_ptr + i)->pd_pres = 0;
                pt_ptr = (((pd_ptr + i)->pd_base) << 12);
                if (frame_bookkeeper[addr_frameid(pt_ptr)].state != FRAME_FREE) {
                    hook_ptable_delete(addr_frameid(pt_ptr) + 1024);
                    frame_bookkeeper[addr_frameid(pt_ptr)].state = FRAME_FREE;
                }
            }
        }
    }

	freestk(prptr->prstkbase, prptr->prstklen);

	switch (prptr->prstate) {
	case PR_CURR:
		prptr->prstate = PR_FREE;	/* Suicide */
		resched();

	case PR_SLEEP:
	case PR_RECTIM:
		unsleep(pid);
		prptr->prstate = PR_FREE;
		break;

	case PR_WAIT:
		semtab[prptr->prsem].scount++;
		/* Fall through */

	case PR_READY:
		getitem(pid);		/* Remove from queue */
		/* Fall through */

	default:
		prptr->prstate = PR_FREE;
	}

	restore(mask);
	return OK;
}
