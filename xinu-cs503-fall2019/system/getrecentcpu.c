/* getrecentcpu.c - getrecentcpu*/

#include <xinu.h>

/*------------------------------------------------------------------------
 *  getrecentcpu -  Return the recent_cpu_i  of a process
 *------------------------------------------------------------------------
 */
fix16_t getrecentcpu(
	  pid32		pid		/* Process ID			*/
	)
{
	intmask	mask;			/* Saved interrupt mask		*/
	fix16_t recent_cpu;			/* recentcpu to return		*/

	mask = disable();
	if (isbadpid(pid)) {
		restore(mask);
		return SYSERR;
	}
	recent_cpu = proctab[pid].recent_cpu_i;
	restore(mask);
	return recent_cpu;
}
