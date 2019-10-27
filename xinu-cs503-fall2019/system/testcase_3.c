#include <xinu.h>

// setting: three MFQS, two PSS, with same init group prio
// expectation: round-robin at group level 

#define L1 10000
#define L2 1000

void t3_proc() {
  int i, j;
  int accu = 0;

  for (i = 0; i < L1; i ++) {
    for (j = 0; j < L2; j ++) {
      accu += i * j;
    }
    /*XTESTCASE_KPRINTF("my pid: %d in the loop\n", currpid);*/
  }
  XTESTCASE_KPRINTF("my pid: %d end\n", currpid);
}  
  

process testcase_3(void) {
  resched_cntl(DEFER_START);

  resume(create(t3_proc, 1024, PSSCHED, 10, "t3_proc_1", 0, NULL));
  resume(create(t3_proc, 1024, PSSCHED, 50, "t3_proc_2", 0, NULL));

  // MFQ has three cuz testcase3 itself is PSS 
  resume(create(t3_proc, 1024, MFQSCHED, 10, "t3_proc_3", 0, NULL));
  resume(create(t3_proc, 1024, MFQSCHED, 10, "t3_proc_4", 0, NULL));
  resume(create(t3_proc, 1024, MFQSCHED, 10, "t3_proc_5", 0, NULL));

  resched_cntl(DEFER_STOP);

  /*return OK;*/
}
