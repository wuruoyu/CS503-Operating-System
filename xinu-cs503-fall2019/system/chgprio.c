/* chgprio.c - changing scheduling group priorities */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  chgprio - changing scheduling group priorities
 *------------------------------------------------------------------------
 */
syscall chgprio(int group, pri16 newprio) {
  if (group != 0 && group != 1) {
    return SYSERR;
  }
  grouptab[group].gprio = newprio;
}
