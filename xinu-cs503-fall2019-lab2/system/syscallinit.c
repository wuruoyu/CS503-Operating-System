/* syscallinit.c - syscallinit(x86) */

#include <xinu.h>

extern	long	defevec[];	/* Default exception vector		*/
#define NID 49

/*------------------------------------------------------------------------
 * syscallinit  -  Initialize the syscall at startup (x86)
 *------------------------------------------------------------------------
 */
void	syscallinit(void)
{
	/* Set interrupt vector to invoke clkdisp */

	/*ioapic_irq2vec(2, 128);*/

	set_evec(0x80, defevec[NID - 1]);
	set_ivec(0x80, syscallhandler, 0);

	return;
}
