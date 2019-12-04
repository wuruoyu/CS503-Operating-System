/* vfreemem.c - vfreemem */

#include <xinu.h>

syscall	vfreemem(
	  char		*blkaddr,	/* Pointer to memory block	*/
	  uint32	nbytes		/* Size of block in bytes	*/
	)
{
    intmask	mask;			/* Saved interrupt mask		*/
    struct  procent *prptr;
	struct	vmemblk	*vmemlist, *next, *prev;
	uint32	top;

	mask = disable();
	if ((nbytes == 0) || ((uint32) blkaddr < (uint32) PRIVATE_HEAP_MIN)
			  || ((uint32) blkaddr > (uint32) PRIVATE_HEAP_MAX)) {
		restore(mask);
        XERROR_KPRINTF("[vfreemem] blkaddr invalid\n");
		return SYSERR;
	}

	nbytes = (uint32) roundmb(nbytes);	/* Use memblk multiples	*/
	/*block = (struct memblk *)blkaddr;*/

    prptr = &proctab[currpid];
    vmemlist = &(prptr->vmemlist);

	prev = vmemlist;			/* Walk along free list	*/
	next = vmemlist->mnext;
	while ((next->mbase != NULL) && (next->mbase < blkaddr)) {
		prev = next;
		next = next->mnext;
	}

	if (prev == &vmemlist) {		/* Compute top of previous block*/
		top = (uint32) NULL;
	} else {
		top = (uint32) prev->mbase + prev->mlength;
	}

	/* Ensure new block does not overlap previous or next blocks	*/

	if (((prev != &vmemlist) && (uint32) blkaddr < top)
	    || ((next != NULL)	&& (uint32)blkaddr + nbytes > (uint32)next->mbase)) {
		restore(mask);
        XERROR_KPRINTF("[vfreemem] block collide with others\n");
		return SYSERR;
	}

	vmemlist->mlength += nbytes;

	/* Either coalesce with previous block or add to free list */

    int flag = 0;

	if (top == (uint32) blkaddr) { 	/* Coalesce with previous block	*/
		prev->mlength += nbytes;
		blkaddr = prev->mbase;
        flag = 0;
	} else {			/* Link into list as new node	*/
		prev->mnext = getmem(sizeof(struct vmemblk));
        (prev->mnext)->mnext = next;
        (prev->mnext)->mlength = nbytes;
        (prev->mnext)->mbase = blkaddr;
        flag = 1;
	}

	/* Coalesce with next block if adjacent */

    if (flag == 0) {
        if (((uint32)blkaddr + prev->mlength) == (uint32) next->mbase) {
            prev->mlength += next->mlength;
            prev->mnext = next->mnext;
            // free next block
            freemem(next, sizeof(struct vmemblk));
        }
    }
    else {
        if (((uint32)blkaddr + (prev->mnext)->mlength) == (uint32) next->mbase) {
            (prev->mnext)->mlength += next->mlength;
            (prev->mnext)->mnext = next->mnext;
            // free next block
            freemem(next, sizeof(struct vmemblk));
        }
    }

    XDEBUG_KPRINTF("[vfreemem] free!\n");
	restore(mask);
	return OK;
}
