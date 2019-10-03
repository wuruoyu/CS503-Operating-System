/* setuid.c - setuid */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  setuid -  Change the uid of a process
 *------------------------------------------------------------------------
 */
syscall setuid(uid16 newuid /* New uid*/
) {
  intmask mask;          /* Saved interrupt mask		*/
  struct procent *prptr; /* Ptr to process's table entry	*/

  mask = disable();

  if (currpid.uid == ROOTUID) {
    currpid.uid = newuid;
    restore(mask);
    return OK;
  } else {
    restore(mask);
    return SYSERR;
  }
}
