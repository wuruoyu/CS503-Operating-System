/* syscalldisp.s - syscalldisp (x86) */

/*------------------------------------------------------------------------
 * syscalldisp  -  Interrupt dispatcher for syscall interrupts (x86 version)
 *------------------------------------------------------------------------
 */
#include <icu.s>
		.text
		.globl	syscalldisp	# syscall interrupt dispatcher
syscalldisp:
		pushal			# Save registers
		cli			# Disable further interrupts
		movb	$EOI,%al	# Reset interrupt
		outb	%al,$OCW1_2

		call	syscallhandler	# Call high level handler

		sti			# Restore interrupt status
		popal			# Restore registers
		iret			# Return from interrupt
