#include <xinu.h>
#include <syscall_interface.h>

void t2_proc1() {
	int i;

	i = getprio(currpid);
      	XDEBUG_KPRINTF("[testcase_2] currpid: %d\n", currpid);
      	XDEBUG_KPRINTF("[testcase_2] prio: %d\n", i);

	// getprio: 1 argument
      	i = syscall_interface(2, 8, currpid);
      	XDEBUG_KPRINTF("[testcase_2] prio: %d\n", i);

	return 1;
}  
  

process testcase_2(void) {
  	resume(create(t2_proc1, 1024, 50, "t2_proc1", 0, NULL));

  	return OK;
}
