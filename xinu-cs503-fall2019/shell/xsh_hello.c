/* xsh_hello.c - xsh_hello */

#include <xinu.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 * xsh_hello - say hello 
 *------------------------------------------------------------------------
 */
shellcmd xsh_hello(void)
{
    hello();
    return 0;
}
