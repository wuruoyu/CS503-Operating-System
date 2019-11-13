#include <xinu.h>

void t1_proc1() {
	bpid32 filebufpool;
	filebufpool = mkbufpool(256, 50);
       /* bpid32 elfbufpool;*/
	/*elfbufpool = mkbufpool(1048576, 8);*/
	int status;

	// file1
	char* file1 = getbuf(filebufpool);
	strncpy(file1, "This is file1\0", 14);
	status = fileregister("/somewhere/file1", file1, 14);

	// filelist for file1
	char* file_list = getbuf(filebufpool);
	filelist(file_list, 256);
	XDEBUG_KPRINTF("[testcase_1] filelist buf: %s\n", file_list);

	// file2
	char* file2 = getbuf(filebufpool);
	strncpy(file2, "This is file2\0", 14);
	fileregister("/elsewhere/file2", file2, 14);

	// filelist
	file_list = getbuf(filebufpool);
	filelist(file_list, 256);
	XDEBUG_KPRINTF("[testcase_1] filelist buf: %s\n", file_list);

	// open
	
	// cannot opened by two proc
	
	// close
	
	// can not close the file you dont hold
	
	// list prefix
	
	// can not unregister a unregistered file
	
	// can not unregister a opend file
	
	// related to unload
}  
  

process testcase_1(void) {
  	resume(create(t1_proc1, 1024, 50, "t1_proc1", 0, NULL));

  	return OK;
}
