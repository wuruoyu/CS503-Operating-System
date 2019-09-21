/* clkhandler.c - clkhandler */

#include <xinu.h>

/*------------------------------------------------------------------------
 * clkhandler - high level clock interrupt handler
 *------------------------------------------------------------------------
 */
void	clkhandler(
		int32	arg	/* Interrupt handler argument	*/
		)
{

	if(!(hpet->gis & HPET_GIS_T0)) {
		return;
	}

	hpet->gis |= HPET_GIS_T0;

	/* Decrement the ms counter, and see if a second has passed */

	if((++count1000) >= 1000) {

		/* One second has passed, so increment seconds count */

		clktime++;

    /* MFQ update */

    // Update recent_cpu_i of every non-null process of the scheduling group
    // Calculate n_ready_processes
    int32 readylist_idx = firstid(readylist);
    int32 n_ready_processes = 0;
    while (nextid(readylist_idx) != EMPTY) {
      if (proctab[readylist_idx].group == MFQSCHED && readylist_idx != NULLPROC) {
	      struct procent *pt = &proctab[readylist_idx];	
        pt->recent_cpu_i *= (2 * load_avg) / (2 * load_avg + 1);
        pt->recent_cpu_i += pt->nice;
        n_ready_processes ++;
      }
    }

    // Update load_avg
    load_avg = (59 / 60) * load_avg + (1 / 60) * n_ready_processes;

		/* Reset the local ms counter for the next second */

		count1000 = 0;
	}

  /* MFQ update */

  // Every 10ms, update priority_i 
  if ((++count10) >= 10) {
    int32 readylist_idx = firstid(readylist);
    while (nextid(readylist_idx) != EMPTY) {
      if (proctab[readylist_idx].group == MFQSCHED) {
	      struct procent *pt = &proctab[readylist_idx];	
        pt->priority_i = 100 - (pt->recent_cpu_i / 2) - (pt->nice * 2);
        if (pt->priority_i > 100)
          pt->priority_i = 100;
        if (pt->priority_i < 0)
          pt->priority_i = 0;
      }
    }
    count10 = 0;
  }

  // Every 1ms, update recent_cpu_i for currpid
  proctab[currpid].recent_cpu_i ++;

  /* Increment the last_resched_ms counter */

  last_resched_ms ++;

	/* Handle sleeping processes if any exist */

	if(!isempty(sleepq)) {

		/* Decrement the delay for the first process on the	*/
		/*   sleep queue, and awaken if the count reaches zero	*/

		if((--queuetab[firstid(sleepq)].qkey) <= 0) {
			wakeup();
		}
	}

	/* Decrement the preemption counter, and reschedule when the */
	/*   remaining time reaches zero			     */

	if((--preempt) <= 0) {
		preempt = QUANTUM;
		resched();
	}
}
