#include <xinu.h>

extern void page_policy_test(void);

void ctest(void) {
    int i = 1;
    XDEBUG_KPRINTF("[ctest] %d\n", (i << 12));
}

static void debug(void) {
  // test identity mapping
  XDEBUG_KPRINTF("[debug] currpid: %d\n", currpid);
  XDEBUG_KPRINTF("[debug] prpdptr: %x\n", proctab[currpid].prpdptr);
  char* mem = vgetmem(8);
  XDEBUG_KPRINTF("[debug] finish vgetmem\n");

  // touch and access
  XDEBUG_KPRINTF("[debug] touch\n");
  int* mem_int = (int*)mem;
  *mem_int = 4;
  XDEBUG_KPRINTF("[debug] value: %d\n", *mem_int);
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

  /*pid32 p = vcreate(debug, INITSTK, 1024,*/
                    /*INITPRIO, "debug", 0, NULL);*/
  /*resume(p);*/

  page_policy_test();

  /*while (1) {*/
    /*if(proctab[p].prstate == PR_FREE) {*/
      /*break;*/
    /*}*/
    /*else {*/
      /*sleepms(100);*/
    /*}*/
  /*}*/

  return OK;
}


