/* filelist.c - list file*/

#include <xinu.h>

/*------------------------------------------------------------------------
 *  filelist - list file 
 *------------------------------------------------------------------------
 */
int filelist(char* buf_list_out, int buf_list_max)
{
	int32 i;
	intmask mask;

	mask = disable();

	int buf_list_counter = 0;
	for (i = 0; i < NFILE; i++) {
		if (filetab[i].filestate == FILE_OCCUPIED) {
			if (buf_list_counter + strlen(filetab[i].filepath) + 1 <= buf_list_max) {
				strncat(buf_list_out, filetab[i].filepath, strlen(filetab[i].filepath));
				buf_list_counter += strlen(filetab[i].filepath);
				buf_list_out[buf_list_counter] = '\n';
				buf_list_counter ++;
			}
			else {
				int local_counter = 0;
				for ( ; buf_list_counter <= buf_list_max; buf_list_counter ++) {
					*(buf_list_out + buf_list_counter) = *(filetab[i].filepath + local_counter);
					local_counter ++;
				}
				XDEBUG_KPRINTF("filelist buf: %s\n", buf_list_out);
				restore(mask);
				return SYSERR;
			}
		}
	}

	XDEBUG_KPRINTF("filelist buf: %s\n", buf_list_out);
	restore(mask);
	return OK; 
}
