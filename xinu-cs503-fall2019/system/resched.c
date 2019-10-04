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

  // Find the smallest pi ready process in the PSS group: traverse the
  // whole list
  int32 iter_idx = firstid(readylist);
  int32 smallest_idx = NULLPROC;
  pri16 smallest_pi = SHRT_MAX;
  while (nextid(iter_idx) != EMPTY) {
    if (PSSCHED == proctab[iter_idx].group &&
        proctab[iter_idx].pi < smallest_pi) {
      smallest_idx = iter_idx;
      smallest_pi = proctab[iter_idx].pi;
    }
    iter_idx = nextid(iter_idx);
  }

  return smallest_idx;
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
    /*XDEBUG_KPRINTF("Select PSS:\n");*/
    return pss();
  } else {
    /*XDEBUG_KPRINTF("Select MFQ:\n");*/
    return mfq();
  }
}

/*------------------------------------------------------------------------
 *  resched  -  Reschedule processor to highest priority eligible process
 *------------------------------------------------------------------------
 */
void resched(void) /* Assumes interrupts are disabled	*/
{
  XDEBUG_KPRINTF("Resched: \n");

  struct procent *ptold; /* Ptr to table entry for old process	*/
  struct procent *ptnew; /* Ptr to table entry for new process	*/

  /* If rescheduling is deferred, record attempt and return */

  if (Defer.ndefers > 0) {
    Defer.attempt = TRUE;
    XDEBUG_KPRINTF("Resched defer\n");
    return;
  }

  /* Update current running process if it belongs to pss */

  if (proctab[currpid].group == PSSCHED) {
    fix16_t ratio = fix16_div(fix16_from_int(100), fix16_from_int(proctab[currpid].prprio));
    fix16_t inc = fix16_mul(ratio, fix16_from_int(last_resched_ms));
    proctab[currpid].pi += fix16_to_int(inc);

    /*XDEBUG_KPRINTF("PSS: curr name = %s\n", proctab[currpid].prname);*/
    /*XDEBUG_KPRINTF("PSS: curr pid = %d\n", currpid);*/
    /*XDEBUG_KPRINTF("PSS: curr prprio = %d\n", proctab[currpid].prprio);*/
    /*XDEBUG_KPRINTF("PSS: curr last_resched_ms = %u\n", last_resched_ms);*/
    /*XDEBUG_KPRINTF("PSS: curr ratio = %f\n", fix16_to_float(ratio));*/
    /*XDEBUG_KPRINTF("PSS: curr inc = %d\n", fix16_to_int(inc));*/
    /*XDEBUG_KPRINTF("PSS: curr pi: %d\n", proctab[currpid].pi);*/
  }

  /* Choose group, return next_pid */

  pid32 next_pid = choose_group();
  /*XDEBUG_KPRINTF("PSS next_pid name: %s\n", proctab[next_pid].prname);*/
  /*XDEBUG_KPRINTF("PSS next_pid : %d\n", next_pid);*/
  /*XDEBUG_KPRINTF("PSS next_pid pi: %d\n", proctab[next_pid].pi);*/
  /*XDEBUG_KPRINTF("PSS next_pid group: %d\n", proctab[next_pid].group);*/

  /* Point to process table entry for the current (old) process */

  ptold = &proctab[currpid];

  /* Process remains eligible if it is still of highest priority in the selected
   * group */
  if (proctab[next_pid].group == PSSCHED) {
    if (ptold->prstate == PR_CURR) {
      if (ptold->pi < proctab[next_pid].pi &&
          proctab[currpid].group == PSSCHED) {
	XDEBUG_KPRINTF("PSS remain\n");
        return;
      }
    }
  } else if (proctab[next_pid].group == MFQSCHED) {
    if (ptold->prstate == PR_CURR) {
      if (ptold->priority_i > proctab[next_pid].priority_i &&
          proctab[currpid].group == MFQSCHED) {
	XDEBUG_KPRINTF("MFQ remain\n");
        return;
      }
    }
  } else {
    XDEBUG_KPRINTF("ERROR\n");
  }

  /* enqueue the old proc into readylist if it remains eligible */
  
  if (ptold->prstate == PR_CURR) {
    ptold->prstate = PR_READY;
    enqueue(currpid, readylist);
  }

  /* PSS update */

  if (proctab[next_pid].group == PSSCHED) {
    if (clktime * 1000 + count1000 > proctab[next_pid].pi) {
      proctab[next_pid].pi = clktime * 1000 + count1000;
    }
  }

  /* Force context switch to highest priority/lowest pi ready process */

  XDEBUG_KPRINTF("resched from process [%s] to the process [%s] \n", proctab[currpid].prname, proctab[next_pid].prname);

  // currpid = dequeue(readylist);
  currpid = getitem(next_pid);
  queuetab[currpid].qprev = EMPTY;
  queuetab[currpid].qnext = EMPTY;
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
