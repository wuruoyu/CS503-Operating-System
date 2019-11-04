/* fileregister.c - register file*/

#include <xinu.h>

/*------------------------------------------------------------------------
 *  fileregister - register file 
 *------------------------------------------------------------------------
 */
int fileregister(char* path, char* file, int file_size)
{
	struct fileent* filepr;
	int32 i;
	intmask mask;

	mask = disable();
	
	// check if file path has been registered
	for (i = 0; i < NFILE; i ++) {
		if (filetab[i].filestate == FILE_OCCUPIED) {
			if (strcmp(filetab[i].filepath, path) == 0) {
				restore(mask);
				return SYSERR;
			}
		}
	}	

	// find a empty slot
	for (i = 0; i < NFILE; i++) {
		if (filetab[i].filestate == FILE_FREE) {
			break;
		}
		// no empty slot
		if (i == NFILE - 1) {
			restore(mask);
			return SYSERR;
		}
	}

	filepr = &filetab[i];
	filepr->filestate = FILE_OCCUPIED;
	filepr->filepath = path;
	filepr->filecontent = file;
	filepr->filesize = file_size;
	filepr->fileopen = FILE_UNOPEN;
	restore(mask);
	return OK;
}
