#include <xinu.h>

extern void page_policy_test(void);

void ctest(void) {
    int i = 1;
    XDEBUG_KPRINTF("[ctest] %d\n", (i << 12));
}

static void debug(void) {
  XDEBUG_KPRINTF("[debug] hw\n");
  char *mem = vgetmem(1);
  XDEBUG_KPRINTF("[debug] finish vgetmem\n");
}

process	main(void)
{
  srpolicy(FIFO);

  /* Start the network */
  /* DO NOT REMOVE OR COMMENT BELOW */
  netstart();

  /*  TODO. Please ensure that your paging is activated 
      by checking the values from the control register.
  */

  /* Initialize the page server */
  /* DO NOT REMOVE OR COMMENT THIS CALL */
  psinit();

  ctest();

  pid32 p = vcreate(debug, INITSTK, 1024,
                    INITPRIO, "debug", 0, NULL);
  /*page_policy_test();*/
  resume(p);

  while (1) {
    if(proctab[p].prstate == PR_FREE) {
      break;
    }
    else {
      sleepms(100);
    }
  }

  return OK;
}


