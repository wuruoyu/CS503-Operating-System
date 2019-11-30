/* vgetmem.c - vgetmem */

#include <xinu.h>


char  	*vgetmem(
	  uint32	nbytes		/* Size of memory requested	*/
	)
{
    intmask	mask;			/* Saved interrupt mask		*/
    struct  procent* prptr;
	struct	memblk	*vmemlist, *prev, *curr, *leftover;

	mask = disable();

    // currpid
    prptr = &proctab[currpid];
    vmemlist = &(prptr->vmemlist);

    XDEBUG_KPRINTF("[vgetmem] vmemlist size: %d\n", vmemlist->mlength);

    // init the vmemlist, cause page fault
    struct memblk *init_vmemlist;
    if (vmemlist->mnext == NULL) {
        vmemlist->mnext = (struct memblk*)PRIVATE_HEAP_MIN;
        // touch
        init_vmemlist = vmemlist->mnext;
        init_vmemlist->mlength = vmemlist->mlength;
        init_vmemlist->mnext = NULL;
    }

	nbytes = (uint32) roundmb(nbytes);	/* Use memblk multiples	*/
    XDEBUG_KPRINTF("[vgetmem] getmem size: %d\n", nbytes);

    // check if it exceeds the limit
    if (vmemlist->mlength < nbytes) {
        XDEBUG_KPRINTF("[vgetmem] size exceeds limit\n");
        restore(mask);
        return (char *)SYSERR;
    }
    if (nbytes == 0) {
        restore(mask);
        return (char *)SYSERR;
    }

    prev = &(prptr->vmemlist);
    curr = (prptr->vmemlist).mnext;
    while (curr != NULL) {			/* Search free list	*/

		if (curr->mlength == nbytes) {	/* Block is exact match	*/
			prev->mnext = curr->mnext;
			vmemlist->mlength -= nbytes;
			restore(mask);
			return (char *)(curr);

		} else if (curr->mlength > nbytes) { /* Split big block	*/
			leftover = (struct memblk *)((uint32) curr +
					nbytes);
			prev->mnext = leftover;
			leftover->mnext = curr->mnext;
			leftover->mlength = curr->mlength - nbytes;
			vmemlist->mlength -= nbytes;
			restore(mask);
			return (char *)(curr);
		} else {			/* Move to next block	*/
			prev = curr;
			curr = curr->mnext;
		}
	}
	restore(mask);
	return (char *)SYSERR;
}
