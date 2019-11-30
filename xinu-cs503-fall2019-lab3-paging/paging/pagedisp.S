/* pagedisp.s - pagedisp (x86) */

/*------------------------------------------------------------------------
 * pagedisp -  Interrupt dispatcher for page fault (x86 version)
 *------------------------------------------------------------------------
 */
#include <icu.s>
		.text
error_code: .long 0
		.globl	pagedisp, error_code		# page fault interrupt dispatcher
pagedisp:
        popl    error_code
		pushal			        # Save registers
        pushfl

		call	pagehandler	    # Call high level handler

        popfl
		popal			        # Restore registers
		iret			        # Return from interrupt
