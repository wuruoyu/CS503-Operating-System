/* resched.c - resched, resched_cntl */

#include <xinu.h>

struct	defer	Defer;

/*------------------------------------------------------------------------
 *  pss - proportional share scheduler, update priority
 *------------------------------------------------------------------------
 */
void pss(void) {

  /* Update current running process */

  if (proctab[currpid].group == PSSCHED) {
    fix16_t ratio = 100 / proctab[currpid].prprio; 
    XDEBUG_KPRINTF("PSS curr ratio: %f", ratio);

    XDEBUG_KPRINTF("PSS curr consume time:  %f", last_resched_ms);

    proctab[currpid].pi += last_resched_ms * ratio;
    proctab[currpid].prprio = INT_MAX - proctab[currpid].pi;
    XDEBUG_KPRINTF("PSS curr updated val: %f", proctab[currpid].pi);
  }
}

/*------------------------------------------------------------------------
 *  mfq - multilevel feedback queue scheduler, update priority
 *------------------------------------------------------------------------
 */
void mfq(void) { 
  /* See clkhandler.c */
}

/*------------------------------------------------------------------------
 *  resched  -  Reschedule processor to highest priority eligible process
 *------------------------------------------------------------------------
 */
void	resched(void)		/* Assumes interrupts are disabled	*/
{
	struct procent *ptold;	/* Ptr to table entry for old process	*/
	struct procent *ptnew;	/* Ptr to table entry for new process	*/

	/* If rescheduling is deferred, record attempt and return */

	if (Defer.ndefers > 0) {
		Defer.attempt = TRUE;
		return;
	}

  /* If current proc belongs to group A, assign initial priority to group A */

  grouptab[proctab[currpid].group].gprio = INITGPRIO;

  /* Incremented by the number of processes in the ready queue */

  int32 readylist_idx = firstid(readylist);
  while (nextid(readylist_idx) != EMPTY) {
    if (readylist_idx != currpid && readylist_idx != NULLPROC) {
      grouptab[proctab[readylist_idx].group].prnum ++;
    }
  } 

  /* TODO: critical? update last resched ms */

  last_resched_ms = 0;
  XDEBUG_KPRINTF("resched: %f", proctab[currpid].pi);

  /* Choose the group and do group related update */

  int selcted_group;
  if (grouptab[PSSCHED].prnum >= grouptab[MFQSCHED].prnum) {
    XDEBUG_KPRINTF("Select PSS:\n");
    selcted_group = PSSCHED;
    pss();
  }
  else {
    XDEBUG_KPRINTF("Select MFQ:\n");
    selcted_group = MFQSCHED;
    mfq();
  }

	/* Point to process table entry for the current (old) process */

	ptold = &proctab[currpid];

  /* Find the highest priority ready process in the selected group */
  
  int32 ava_readylist_idx = firstid(readylist);
  while (nextid(ava_readylist_idx) != EMPTY) {
    if (selcted_group == proctab[ava_readylist_idx].group) 
      break;
  }

  /* Process remains eligible if it is still of highest priority in the selected group */
  
	if (ptold->prstate == PR_CURR) {  
    if (ptold->prprio > queuekey(ava_readylist_idx) || proctab[ava_readylist_idx].group != selcted_group)
      return;
  }

  /* Old process will no longer remain current */

  ptold->prstate = PR_READY;
  insert(currpid, readylist, ptold->prprio);

  /* PSS update */

  if (clktime * 1000 + count1000 > proctab[ava_readylist_idx].pi) {
    proctab[ava_readylist_idx].pi = clktime * 1000 + count1000;
    proctab[ava_readylist_idx].prprio = INT_MAX - proctab[ava_readylist_idx].pi;
  }

	/* Force context switch to highest priority ready process */

	// currpid = dequeue(readylist);
  currpid = getitem(ava_readylist_idx);
  queuetab[ava_readylist_idx].qprev = EMPTY; 
  queuetab[ava_readylist_idx].qnext = EMPTY; 
	ptnew = &proctab[currpid];
	ptnew->prstate = PR_CURR;
	preempt = QUANTUM;		/* Reset time slice for process	*/
	ctxsw(&ptold->prstkptr, &ptnew->prstkptr);

	/* Old process returns here when resumed */

	return;
}

/*------------------------------------------------------------------------
 *  resched_cntl  -  Control whether rescheduling is deferred or allowed
 *------------------------------------------------------------------------
 */
status	resched_cntl(		/* Assumes interrupts are disabled	*/
	  int32	defer		/* Either DEFER_START or DEFER_STOP	*/
	)
{
	switch (defer) {

	    case DEFER_START:	/* Handle a deferral request */

		if (Defer.ndefers++ == 0) {
			Defer.attempt = FALSE;
		}
		return OK;

	    case DEFER_STOP:	/* Handle end of deferral */
		if (Defer.ndefers <= 0) {
			return SYSERR;
		}
		if ( (--Defer.ndefers == 0) && Defer.attempt ) {
			resched();
		}
		return OK;

	    default:
		return SYSERR;
	}
}
