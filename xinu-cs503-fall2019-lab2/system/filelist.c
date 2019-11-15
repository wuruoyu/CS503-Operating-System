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

	*buf_list_out = '\0';
	int buf_list_counter = 0;
	for (i = 0; i < NFILE; i++) {
		if (filetab[i].filestate == FILE_OCCUPIED) {
			if (buf_list_counter + strlen(filetab[i].filepath) <= buf_list_max) {
				int j;
				for (j = 0; j < strlen(filetab[i].filepath); j ++) {
					buf_list_out[buf_list_counter + j] = filetab[i].filepath[j];
				}

				buf_list_counter += (strlen(filetab[i].filepath));
				buf_list_out[buf_list_counter] = '\n';
				buf_list_counter ++;
			}
			else {
				int local_counter = 0;
				for ( ; buf_list_counter < buf_list_max; buf_list_counter ++) {
					*(buf_list_out + buf_list_counter) = *(filetab[i].filepath + local_counter);
					local_counter ++;
				}
				buf_list_out[buf_list_counter] = '\0';
				XDEBUG_KPRINTF("[filelist] prefix: %s\n", buf_list_out);
				restore(mask);
				return SYSERR;
			}
		}
	}
	buf_list_out[buf_list_counter] = '\0';

	restore(mask);
	return OK; 
}
