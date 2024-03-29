/* fileopen.c - open file*/

#include <xinu.h>

/*------------------------------------------------------------------------
 *  fileopen - register file 
 *------------------------------------------------------------------------
 */
char* fileopen(char* path, int* file_size_out)
{
	int32 i;
	intmask mask;

	mask = disable();

	// check if file path has been registered
	for (i = 0; i < NFILE; i ++) {
		if (filetab[i].filestate == FILE_OCCUPIED) {
			/*XDEBUG_KPRINTF("[fileopen] filetab[i].filepath: %s\n", filetab[i].filepath);*/
			/*XDEBUG_KPRINTF("[fileopen] path: %s\n", path);*/
			if (strcmp(filetab[i].filepath, path) == 0) {
				/*XDEBUG_KPRINTF("[fileopen] match\n");*/
				if (filetab[i].fileopen == FILE_OPEN) {
					restore(mask);
					return SYSERR;
				}
				*file_size_out = filetab[i].filesize; 
				restore(mask);
				return filetab[i].filecontent;
			}
		}
	}	

	restore(mask);
	return SYSERR;
}
