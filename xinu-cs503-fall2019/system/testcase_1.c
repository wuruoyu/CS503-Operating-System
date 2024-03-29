#include <xinu.h>

// setting: two proc in PSSCHED, with same prio
// expectation: round-robin

#define L1 10000
#define L2 1000

void t1_proc() {
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
  

process testcase_1(void) {
  resched_cntl(DEFER_START);

  resume(create(t1_proc, 1024, PSSCHED, 50, "t1_proc_1", 0, NULL));
  resume(create(t1_proc, 1024, PSSCHED, 50, "t1_proc_2", 0, NULL));

  resched_cntl(DEFER_STOP);

  return OK;
}
