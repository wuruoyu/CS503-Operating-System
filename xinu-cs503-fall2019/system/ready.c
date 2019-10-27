/* ready.c - ready */

#include <xinu.h>

qid16 readylist; /* Index of ready list		*/

/*------------------------------------------------------------------------
 *  ready  -  Make a process eligible for CPU service
 *------------------------------------------------------------------------
 */
status ready(pid32 pid /* ID of process to make ready	*/
) {
  register struct procent *prptr;

  if (isbadpid(pid)) {
    return SYSERR;
  }

  /* PSS update */

  if (proctab[pid].group == PSSCHED) {
    if (clktime * 1000 + count1000 > proctab[pid].pi) {
      proctab[pid].pi = clktime * 1000 + count1000;
    }
  }

  /* Set process state to indicate ready and add to ready list */

  prptr = &proctab[pid];
  prptr->prstate = PR_READY;
  insert(pid, readylist, prptr->prprio);
  resched();

  return OK;
}
