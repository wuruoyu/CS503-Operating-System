/* hello.c - hello */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  hello - HELLO WORLD!
 *------------------------------------------------------------------------
 */
syscall hello(void) {
  kprintf("Hello system call invoked\n");
  return 1;
}
