#include <xinu.h>

void t1_proc1() {
	bpid32 filebufpool;
	filebufpool = mkbufpool(256, 50);
       /* bpid32 elfbufpool;*/
	/*elfbufpool = mkbufpool(1048576, 8);*/
	int status;

	// file1
	char* file1 = getbuf(filebufpool);
	strncpy(file1, "This is file1", 14);
	status = fileregister("/somewhere/file1", file1, 14);

	// filelist for file1
	/*char* file_list = getbuf(filebufpool);*/
	/*filelist(file_list, 256);*/
	/*XDEBUG_KPRINTF("[testcase_1] filelist buf: %s\n", file_list);*/

	// file2
	char* file2 = getbuf(filebufpool);
	strncpy(file2, "This is file2", 14);
	fileregister("/elsewhere/file2", file2, 14);

	// filelist
	char* file_list_2 = getbuf(filebufpool);
	filelist(file_list_2, 256);
	XDEBUG_KPRINTF("[testcase_1] filelist buf: %s\n", file_list_2);

	// open
	int file1_size_t = 0;
	char* file1_t = fileopen("/somewhere/file1", &file1_size_t);
	if ((char*)SYSERR == file1_t) {
		XDEBUG_KPRINTF("[testcase_1] open file1 FAIL\n");
	}
	XDEBUG_KPRINTF("[testcase_1] file1_t: %s\n", file1_t);
	if (file1_size_t != 14) {
		XDEBUG_KPRINTF("[testcase_1] ERR: fileopen file1 size wrong\n");
	}
	if (strcmp(file1_t, "This is file1") != 0) {
		XDEBUG_KPRINTF("[testcase_1] ERR: fileopen file1 content wrong\n");
	}
	
	// open a non-exist file
	int file_n_size = 0;
	char* file_n = fileopen("nowhere/file1", &file_n_size);
	if ((char*)SYSERR != file_n) {
		XDEBUG_KPRINTF("[testcase_1] file should not exist");
	}

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
