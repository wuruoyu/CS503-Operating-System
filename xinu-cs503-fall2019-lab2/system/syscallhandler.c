/* syscallhandler.c - syscallhandler */

#include <xinu.h>

#define OFFSET 12

/*------------------------------------------------------------------------
 * syscallhandler - high level syscall interrupt handler
 *------------------------------------------------------------------------
 */
syscall syscallhandler(
		int32	arg,		/* Interrupt handler argument	*/
		long*	savedsp		/* Hopefully ESP	*/
		)
{
    XDEBUG_KPRINTF("[syscallhandler] *savedsp: %d\n", *(savedsp+12));	
	/*XDEBUG_KPRINTF("[syscallhandler] *savedsp: %d\n", *(savedsp+13));	*/
	/*XDEBUG_KPRINTF("[syscallhandler] *savedsp: %d\n", *(savedsp+14));	*/
	/*XDEBUG_KPRINTF("[syscallhandler] *savedsp: %d\n", *(savedsp+15));	*/
	/*XDEBUG_KPRINTF("[syscallhandler] *savedsp: %d\n", *(savedsp+16));	*/
	/*XDEBUG_KPRINTF("[syscallhandler] *savedsp: %d\n", *(savedsp+17));	*/

	/*XDEBUG_KPRINTF("[syscallhandler] *savedsp: %d\n", *(savedsp+1));	*/
	/*XDEBUG_KPRINTF("[syscallhandler] *savedsp: %d\n", *(savedsp+2));	*/
	/*XDEBUG_KPRINTF("[syscallhandler] *savedsp: %d\n", *(savedsp+3));	*/
	/*XDEBUG_KPRINTF("[syscallhandler] *savedsp: %d\n", *(savedsp+4));	*/
	/*XDEBUG_KPRINTF("[syscallhandler] *savedsp: %d\n", *(savedsp+5));	*/
	/*XDEBUG_KPRINTF("[syscallhandler] *savedsp: %d\n", *(savedsp+6));	*/
	/*XDEBUG_KPRINTF("[syscallhandler] *savedsp: %d\n", *(savedsp+7));	*/
	/*XDEBUG_KPRINTF("[syscallhandler] *savedsp: %d\n", *(savedsp+8));	*/
	/*XDEBUG_KPRINTF("[syscallhandler] *savedsp: %d\n", *(savedsp+9));	*/
	/*XDEBUG_KPRINTF("[syscallhandler] *savedsp: %d\n", *(savedsp+10));	*/

	if (*(savedsp + OFFSET) == 1) {
		return syscall_interface_tab[*(savedsp + OFFSET + 1)].funcaddr();
	}
	else if (*(savedsp + OFFSET) == 2) {
		int ret = syscall_interface_tab[*(savedsp + OFFSET + 1)].funcaddr(
				*(savedsp + OFFSET + 2)); 
		return ret;
	}
	else if (*(savedsp + OFFSET) == 3) {
		return syscall_interface_tab[*(savedsp + OFFSET + 1)].funcaddr(
				*(savedsp + OFFSET + 2), 
				*(savedsp + OFFSET + 3)); 
	}
	else if (*(savedsp + OFFSET) == 4) {
		syscall_interface_tab[*(savedsp + OFFSET + 1)].funcaddr(
				*(savedsp + OFFSET + 2), 
				*(savedsp + OFFSET + 3), 
				*(savedsp + OFFSET + 4)); 
	}
	else if (*(savedsp + OFFSET) == 5) {
		syscall_interface_tab[*(savedsp + OFFSET + 1)].funcaddr(
				*(savedsp + OFFSET + 2), 
				*(savedsp + OFFSET + 3), 
				*(savedsp + OFFSET + 4), 
				*(savedsp + OFFSET + 5)); 
	}
	else {
		XDEBUG_KPRINTF("[syscallhandler] too much argument\n");
		return SYSERR;
	}

}
