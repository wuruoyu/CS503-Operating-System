/* vgetmem.c - vgetmem */

#include <xinu.h>


char  	*vgetmem(
	  uint32	nbytes		/* Size of memory requested	*/
	)
{
    intmask	mask;			/* Saved interrupt mask		*/
    struct  procent* prptr;
	struct	vmemblk	*vmemlist, *prev, *curr, *leftover;

	mask = disable();

    // currpid
    prptr = &proctab[currpid];
    vmemlist = &(prptr->vmemlist);

    XDEBUG_KPRINTF("[vgetmem] vmemlist size: %d\n", vmemlist->mlength);

    // I still wanna it be aligned
    nbytes = (uint32) roundmb(nbytes);	/* Use memblk multiples	*/

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

    // init the vmemlist
    struct vmemblk *temp_vmemlist;
    if (vmemlist->mnext == NULL) {
        /*vmemlist->mnext = (struct memblk*)PRIVATE_HEAP_MIN;*/
        vmemlist->mnext = getmem(sizeof(struct vmemblk));
        temp_vmemlist = vmemlist->mnext;
        temp_vmemlist->mlength = vmemlist->mlength;
        temp_vmemlist->mnext = NULL;
        temp_vmemlist->mbase = (char*)PRIVATE_HEAP_MIN;
    }

    XDEBUG_KPRINTF("[vgetmem] getmem size: %d\n", nbytes);

    prev = &(prptr->vmemlist);
    curr = (prptr->vmemlist).mnext;
    while (curr != NULL) {			/* Search free list	*/

		if (curr->mlength == nbytes) {	/* Block is exact match	*/
			prev->mnext = curr->mnext;
			vmemlist->mlength -= nbytes;
            char* ret_addr = curr->mbase;
            freemem(curr, sizeof(struct vmemblk));
			restore(mask);
			return ret_addr;

		} 
        else if (curr->mlength > nbytes) { /* Split big block	*/
            leftover = getmem(sizeof(struct vmemblk));
			/*leftover = (struct memblk *)((uint32) curr +*/
					/*nbytes);*/
			prev->mnext = leftover;
			leftover->mnext = curr->mnext;
			leftover->mlength = curr->mlength - nbytes;
            leftover->mbase = curr->mbase + nbytes;
			vmemlist->mlength -= nbytes;
            char* ret_addr = curr->mbase;
            freemem(curr, sizeof(struct vmemblk));
			restore(mask);
			return ret_addr;
		} 
        else {			/* Move to next block	*/
			prev = curr;
			curr = curr->mnext;
		}
	}
	restore(mask);
	return (char *)SYSERR;
}
