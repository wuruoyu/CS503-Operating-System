/* chnice.c - chnice */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  chprio  -  Change the niceness of a process
 *------------------------------------------------------------------------
 */
nid16 chnice(
	  pid32		pid,		/* ID of process to change	*/
	  nid16 newnice		/* New niceness */
	)
{
	intmask	mask;			/* Saved interrupt mask		*/
	struct	procent *prptr;		/* Ptr to process's table entry	*/
	pri16	oldnice;		/* Niceness to return		*/

	mask = disable();
	if (isbadpid(pid)) {
		restore(mask);
		return (nid16) SYSERR;
	}
	prptr = &proctab[pid];
	oldnice = prptr->nice;
	prptr->nice = newnice;
  prptr->priority_i = 100 - (prptr->recent_cpu_i / 2) - (prptr->nice * 2);
  if (prptr->priority_i > 100)
    prptr->priority_i = 100;
  if (prptr->priority_i < 0)
    prptr->priority_i = 0;
	restore(mask);
	return oldnice;
}
