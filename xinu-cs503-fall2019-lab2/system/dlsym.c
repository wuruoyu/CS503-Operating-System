/* dlsym.c - */

#include <xinu.h>

syscall dlsym(void* handle, const char* symbol)
{
	int32 i;
	intmask mask;

	mask = disable();

	// check if current process has opened it
	
	// call load(), if no enough memory, return ERR
	
	// file does not exist or not a valid library

	restore(mask);
	return SYSERR;
}
