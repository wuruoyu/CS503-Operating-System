/* vcreate.c - vcreate */

#include <xinu.h>

local	int newpid();

#define	roundew(x)	( (x+3)& ~0x3)

/*----------------------------------------------------------------------------
 *  vcreate  -  Creates a new XINU process. The process's heap will be private
 *  and reside in virtual memory.
 *----------------------------------------------------------------------------
 */
pid32	vcreate(
	  void		*funcaddr,	/* Address of the function	*/
	  uint32	ssize,		/* Stack size in words		*/
		uint32	hsize,		/* Heap size in num of pages */
	  pri16		priority,	/* Process priority > 0		*/
	  char		*name,		/* Name (for debugging)		*/
	  uint32	nargs,		/* Number of args that follow	*/
	  ...
	)
{
	uint32		savsp, *pushsp;
	intmask 	mask;    	/* Interrupt mask		*/
	pid32		pid;		/* Stores new process id	*/
	struct	procent	*prptr;		/* Pointer to proc. table entry */
	int32		i;
	uint32		*a;		/* Points to list of args	*/
	uint32		*saddr;		/* Stack address		*/

	mask = disable();

    // allocate stack
	if (ssize < MINSTK)
		ssize = MINSTK;
	ssize = (uint32) roundew(ssize);
	if (((saddr = (uint32 *)getstk(ssize)) ==
	    (uint32 *)SYSERR ) ||
	    (pid=newpid()) == SYSERR || priority < 1 ) {
		restore(mask);
		return SYSERR;
	}
    XDEBUG_KPRINTF("[vcreate] creating process %d\n", pid);

    // set up pd
    frameid_t fid;
    fid = find_free_frame();
    if (fid == SYSERR) {
        XERROR_KPRINTF("[vcreate] no free frame\n");
		restore(mask);
        return SYSERR;
    }
    init_pd(fid);
    XDEBUG_KPRINTF("[vcreate] set up pd\n");

    // set up bs
    uint32 left_size = hsize;
    bsd_t bs_id;
    int vpage_counter = 0;
    if (left_size > bstab_get_remaining_page()) {
        XERROR_KPRINTF("[vcreate] we dont have enough bp\n");
        restore(mask);
        return SYSERR;
    }
    while (left_size > MAX_PAGES_PER_BS) {
        bs_id = allocate_bs(MAX_PAGES_PER_BS);
        bstab[bs_id].pid = pid;
        bstab[bs_id].vpage = vpage_counter;
        bstab[bs_id].npages = MAX_PAGES_PER_BS;
        vpage_counter += MAX_PAGES_PER_BS;
        left_size -= MAX_PAGES_PER_BS;
        XDEBUG_KPRINTF("[vcreate] allocate one bs\n");
    }
    XDEBUG_KPRINTF("[vcreate] allocate one bs\n");
    bs_id = allocate_bs(left_size);
    bstab[bs_id].pid = pid;
    bstab[bs_id].vpage = vpage_counter;
    bstab[bs_id].npages = MAX_PAGES_PER_BS;
    XDEBUG_KPRINTF("[vcreate] set up bs\n");

	prcount++;
	prptr = &proctab[pid];

	/* Initialize process table entry for new process */
	prptr->prstate = PR_SUSP;	/* Initial state is suspended	*/
	prptr->prprio = priority;
	prptr->prstkbase = (char *)saddr;
	prptr->prstklen = ssize;
	prptr->prname[PNMLEN-1] = NULLCH;
	for (i=0 ; i<PNMLEN-1 && (prptr->prname[i]=name[i])!=NULLCH; i++)
		;
	prptr->prsem = -1;
	prptr->prparent = (pid32)getpid();
	prptr->prhasmsg = FALSE;
    // LAB3 
    prptr->prpdptr = (pd_t*)frameid_addr(fid);
    prptr->vsize = hsize;

    /* Initialize the vmemlist, dont touch the real thing */
    struct vmemblk* vmemptr;
    vmemptr = &(prptr->vmemlist);
    vmemptr->mlength = hsize * NBPG;
    vmemptr->mnext = NULL;
    vmemptr->mbase = 0;

	/* Set up stdin, stdout, and stderr descriptors for the shell	*/

	prptr->prdesc[0] = CONSOLE;
	prptr->prdesc[1] = CONSOLE;
	prptr->prdesc[2] = CONSOLE;

	/* Initialize stack as if the process was called		*/

	*saddr = STACKMAGIC;
	savsp = (uint32)saddr;

	/* Push arguments */
	a = (uint32 *)(&nargs + 1);	/* Start of args		*/
	a += nargs -1;			/* Last argument		*/
	for ( ; nargs > 0 ; nargs--)	/* Machine dependent; copy args	*/
		*--saddr = *a--;	/*   onto created process' stack*/
	*--saddr = (long)INITRET;	/* Push on return address	*/

	/* The following entries on the stack must match what ctxsw	*/
	/*   expects a saved process state to contain: ret address,	*/
	/*   ebp, interrupt mask, flags, registerss, and an old SP	*/

	*--saddr = (long)funcaddr;	/* Make the stack look like it's*/
					/*   half-way through a call to	*/
					/*   ctxsw that "returns" to the*/
					/*   new process		*/
	*--saddr = savsp;		/* This will be register ebp	*/
					/*   for process exit		*/
	savsp = (uint32) saddr;		/* Start of frame for ctxsw	*/
	*--saddr = 0x00000200;		/* New process runs with	*/
					/*   interrupts enabled		*/

	/* Basically, the following emulates an x86 "pushal" instruction*/

	*--saddr = 0;			/* %eax */
	*--saddr = 0;			/* %ecx */
	*--saddr = 0;			/* %edx */
	*--saddr = 0;			/* %ebx */
	*--saddr = 0;			/* %esp; value filled in below	*/
	pushsp = saddr;			/* Remember this location	*/
	*--saddr = savsp;		/* %ebp (while finishing ctxsw)	*/
	*--saddr = 0;			/* %esi */
	*--saddr = 0;			/* %edi */
	*pushsp = (unsigned long) (prptr->prstkptr = (char *)saddr);
	
	restore(mask);
    XDEBUG_KPRINTF("[vcreate] finish\n");
	return pid;
}

/*------------------------------------------------------------------------
 *  newpid  -  Obtain a new (free) process ID
 *------------------------------------------------------------------------
 */
local	pid32	newpid(void)
{
	uint32	i;			/* Iterate through all processes*/
	static	pid32 nextpid = 1;	/* Position in table to try or	*/
					/*   one beyond end of table	*/

	/* Check all NPROC slots */

	for (i = 0; i < NPROC; i++) {
		nextpid %= NPROC;	/* Wrap around to beginning */
		if (proctab[nextpid].prstate == PR_FREE) {
			return nextpid++;
		} else {
			nextpid++;
		}
	}
	return (pid32) SYSERR;
}
