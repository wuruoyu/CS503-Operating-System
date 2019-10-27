#include <xinu.h>

// setting: two proc in MFQ, different niceness
// expectation: t5_proc_2 should have more share

#define L1 10000
#define L2 1000

void t5_proc() {
  int i, j;
  int accu = 0;

  for (i = 0; i < L1; i ++) {
    for (j = 0; j < L2; j ++) {
      accu += i * j;
    }
    /*XTESTCASE_KPRINTF("my pid: %d in the loop\n", currpid);*/
    /*XTESTCASE_KPRINTF("recent_cpu_i: %d\n", fix16_to_float(getrecentcpu(currpid)));*/
    /*XTESTCASE_KPRINTF("load_avg: %f\n", fix16_to_float(getloadavg()));*/
  }
  XTESTCASE_KPRINTF("my pid: %d end\n", currpid);
}  
  

process testcase_5(void) {
  resched_cntl(DEFER_START);

  resume(create(t5_proc, 1024, MFQSCHED, 50, "t5_proc_1", 0, NULL));
  pid32 t5_proc_2 = create(t5_proc, 1024, MFQSCHED, 50, "t5_proc_2", 0, NULL);
  resume(t5_proc_2);

  chnice(t5_proc_2, -20);

  resched_cntl(DEFER_STOP);

  /*return OK;*/
}
