#include <xinu.h>
#include <syscall_interface.h>

extern char apphello_contents[];
extern int apphello_contents_size;

void t4_proc1() {
	int status;

	// hello elf
	char* hello_elf = apphello_contents;
	fileregister("/elf/hello_elf", hello_elf, apphello_contents_size);

	XDEBUG_KPRINTF("[testcase_3] hello elf content: %s\n", hello_elf);
	XDEBUG_KPRINTF("[testcase_3] hello elf size: %d\n", apphello_contents_size);

	// check if registered
	bpid32 filebufpool;
	filebufpool = mkbufpool(256, 128);
	char* file_list = getbuf(filebufpool);
	filelist(file_list, 256);
	XDEBUG_KPRINTF("[testcase_3] file list: %s\n", file_list);

	// dlopen
	dlopen("/elf/hello_elf");

	// unload
	/*unload(ld_stats.ld_text_addr);*/

	return 1;
}  
  

process testcase_4(void) {
  	resume(create(t4_proc1, 1024, 50, "t4_proc1", 0, NULL));

  	return OK;
}
