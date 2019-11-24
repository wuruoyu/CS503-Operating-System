/* pagedisp.s - pagedisp (x86) */

/*------------------------------------------------------------------------
 * pagedisp -  Interrupt dispatcher for page fault (x86 version)
 *------------------------------------------------------------------------
 */
#include <icu.s>
		.text
		.globl	pagedisp		# page fault interrupt dispatcher
pagedisp:
		pushal			        # Save registers
        pushfl

		call	pagehandler	    # Call high level handler

        popfl
		popal			        # Restore registers
        add $0x4, %esp
		iret			        # Return from interrupt
