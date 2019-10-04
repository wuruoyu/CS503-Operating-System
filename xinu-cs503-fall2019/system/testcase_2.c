#include <xinu.h>

// setting: two proc in PSSCHED, with different prio
// expectation: proc of bigger rate has bigger share 

#define L1 10000
#define L2 1000

void t2_proc() {
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
  

process testcase_2(void) {
  resched_cntl(DEFER_START);

  resume(create(t2_proc, 1024, PSSCHED, 10, "t2_proc_1", 0, NULL));
  resume(create(t2_proc, 1024, PSSCHED, 50, "t2_proc_2", 0, NULL));

  resched_cntl(DEFER_STOP);

  /*return OK;*/
}
