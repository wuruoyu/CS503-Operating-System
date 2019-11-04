/* fileunregister.c - unregister file*/

#include <xinu.h>

/*------------------------------------------------------------------------
 *  file unregister - unregister file 
 *------------------------------------------------------------------------
 */
int fileunregister(char* path)
{
	struct fileent* filepr;
	int32 i;
	intmask mask;

	mask = disable();

	// check if file path has been registered
	for (i = 0; i < NFILE; i ++) {
		if (filetab[i].filestate == FILE_OCCUPIED) {
			if (strcmp(filetab[i].filepath, path) == 0) {
				filetab[i].filestate = FILE_FREE;	
				restore(mask);
				return OK;
			}
		}
	}	

	restore(mask);
	return SYSERR;
}
