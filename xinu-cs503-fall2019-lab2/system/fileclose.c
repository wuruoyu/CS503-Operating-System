/* fileclose.c - close file*/

#include <xinu.h>

/*------------------------------------------------------------------------
 *  fileclose - clos efile 
 *------------------------------------------------------------------------
 */
int fileclose(char* path)
{
	int32 i;
	intmask mask;

	mask = disable();

	for (i = 0; i < NFILE; i ++) {
		if (filetab[i].filestate == FILE_OCCUPIED) {
			if (strcmp(filetab[i].filepath, path) == 0) {
				if (filetab[i].fileopen == FILE_OPEN) {
					restore(mask);
					return OK;
				}
				else {
					restore(mask);
					return SYSERR;
				}
			}
		}
	}	

	restore(mask);
	return SYSERR;

}
