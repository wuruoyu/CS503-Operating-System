#include <xinu.h>

syscall dlsym (void *handle, const char *symbol)
{
	return OK;
}

syscall dlclose( char * handle){
        return OK;
}

syscall dlopen(char* library_file_path){
	return OK;
}

