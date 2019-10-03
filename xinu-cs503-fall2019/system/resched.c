/* resched.c - resched, resched_cntl */

#include <xinu.h>

struct defer Defer;

/*------------------------------------------------------------------------
 *  pss - Proportional Share Scheduler, return the next pid
 *------------------------------------------------------------------------
 */
pid32 pss() {
  // TODO: critical? update last resched ms
  last_resched_ms = 0;

  // Find the highest priority ready process in the selected group: traverse the
  // whole list
  int32 iter_idx = firstid(readylist);
  int32 highest_idx = NULLPROC;
  pri16 highest_pri = SHRT_MIN;
  while (nextid(iter_idx) != EMPTY) {
    if (PSSCHED == proctab[iter_idx].group &&
        proctab[iter_idx].prprio > highest_pri) {
      highest_idx = iter_idx;
      highest_pri = proctab[iter_idx].prprio;
    }
    iter_idx = nextid(iter_idx);
  }

  return highest_idx;
}

/*------------------------------------------------------------------------
 *  mfq - Multilevel feedback queue sccheduler, return the next pid
 *------------------------------------------------------------------------
 */
pid32 mfq() {
  // Find the highest priority_i ready process in the selected group: traverse
  // the whole list
  int32 iter_idx = firstid(readylist);
  int32 highest_idx = NULLPROC;
  pri16 highest_priority_i = SHRT_MIN;
  while (nextid(iter_idx) != EMPTY) {
    if (MFQSCHED == proctab[iter_idx].group &&
        proctab[iter_idx].priority_i > highest_priority_i) {
      highest_idx = iter_idx;
      highest_priority_i = proctab[iter_idx].priority_i;
    }
    iter_idx = nextid(iter_idx);
  }

  return highest_idx;
}

/*------------------------------------------------------------------------
 *  choose_group - Choose group, return the next pid
 *------------------------------------------------------------------------
 */
pid32 choose_group() {
  /* If current proc belongs to group A, assign initial priority to group A */

  grouptab[proctab[currpid].group].gprio = INITGPRIO;

  /* Incremented by the number of processes in the ready queue */

  int32 readylist_idx = firstid(readylist);
  while (nextid(readylist_idx) != EMPTY) {
    if (readylist_idx != currpid && readylist_idx != NULLPROC) {
      grouptab[proctab[readylist_idx].group].prnum++;
    }
    readylist_idx = nextid(readylist_idx);
  }

  /* determine group */
  if (grouptab[PSSCHED].prnum >= grouptab[MFQSCHED].prnum) {
    XDEBUG_KPRINTF("Select PSS:\n");
    return pss();
  } else {
    XDEBUG_KPRINTF("Select MFQ:\n");
    return mfq();
  }
}

/*------------------------------------------------------------------------
 *  resched  -  Reschedule processor to highest priority eligible process
 *------------------------------------------------------------------------
 */
void resched(void) /* Assumes interrupts are disabled	*/
{
  struct procent *ptold; /* Ptr to table entry for old process	*/
  struct procent *ptnew; /* Ptr to table entry for new process	*/

  /* If rescheduling is deferred, record attempt and return */

  if (Defer.ndefers > 0) {
    Defer.attempt = TRUE;
    return;
  }

  /* Update current running process if it belongs to pss */

  if (proctab[currpid].group == PSSCHED) {
    fix16_t ratio = 100 / proctab[currpid].prprio;
    XDEBUG_KPRINTF("PSS curr ratio: %f", ratio);

    XDEBUG_KPRINTF("PSS curr consume time:  %f", last_resched_ms);

    proctab[currpid].pi += last_resched_ms * ratio;
    proctab[currpid].prprio = INT_MAX - proctab[currpid].pi;
    XDEBUG_KPRINTF("PSS curr updated val: %f", proctab[currpid].pi);
  }

  /* Choose group */

  pid32 next_pid = choose_group();

  /* Point to process table entry for the current (old) process */

  ptold = &proctab[currpid];

  /* Process remains eligible if it is still of highest priority in the selected
   * group */
  if (proctab[next_pid].group == PSSCHED) {
    if (ptold->prstate == PR_CURR) {
      if (ptold->prprio > proctab[next_pid].prprio &&
          proctab[currpid].group == PSSCHED)
        return;
    }
  } else if (proctab[next_pid].group == MFQSCHED) {
    if (ptold->prstate == PR_CURR) {
      if (ptold->priority_i > proctab[next_pid].priority_i &&
          proctab[currpid].group == MFQSCHED)
        return;
    }
  } else {
    XDEBUG_KPRINTF("ERROR"\n);
  }

  /* Old process will no longer remain current */

  ptold->prstate = PR_READY;
  enqueue(currpid, readylist);

  /* PSS update */

  if (proctab[next_pid].group == PSSCHED) {
    if (clktime * 1000 + count1000 > proctab[next_pid].pi) {
      proctab[next_pid].pi = clktime * 1000 + count1000;
      proctab[next_pid].prprio = INT_MAX - proctab[next_pid].pi;
    }
  }

  /* Force context switch to highest priority ready process */

  // currpid = dequeue(readylist);
  currpid = getitem(next_pid);
  queuetab[next_pid].qprev = EMPTY;
  queuetab[next_pid].qnext = EMPTY;
  ptnew = &proctab[currpid];
  ptnew->prstate = PR_CURR;
  preempt = QUANTUM; /* Reset time slice for process	*/
  ctxsw(&ptold->prstkptr, &ptnew->prstkptr);

  /* Old process returns here when resumed */

  return;
}

/*------------------------------------------------------------------------
 *  resched_cntl  -  Control whether rescheduling is deferred or allowed
 *------------------------------------------------------------------------
 */
status resched_cntl(            /* Assumes interrupts are disabled	*/
                    int32 defer /* Either DEFER_START or DEFER_STOP	*/
) {
  switch (defer) {
    case DEFER_START: /* Handle a deferral request */

      if (Defer.ndefers++ == 0) {
        Defer.attempt = FALSE;
      }
      return OK;

    case DEFER_STOP: /* Handle end of deferral */
      if (Defer.ndefers <= 0) {
        return SYSERR;
      }
      if ((--Defer.ndefers == 0) && Defer.attempt) {
        resched();
      }
      return OK;

    default:
      return SYSERR;
  }
}
