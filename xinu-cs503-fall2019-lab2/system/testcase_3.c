#include <xinu.h>
#include <syscall_interface.h>

extern char apphello_contents[];
extern int apphello_contents_size;

void t3_proc1() {
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

	// loading
	struct load_t ld_stats;
	status = load("/elf/hello_elf", &ld_stats);

	// get ld_stats
	XDEBUG_KPRINTF("[testcase_3] start_addr: %x\n", ld_stats._start_addr);
	XDEBUG_KPRINTF("[testcase_3] ld_text_addr: %x\n", ld_stats.ld_text_addr);
	XDEBUG_KPRINTF("[testcase_3] ld_text_size: %d\n", ld_stats.ld_text_size);
	XDEBUG_KPRINTF("[testcase_3] ld_bss_addr: %x\n", ld_stats.ld_bss_addr);
	XDEBUG_KPRINTF("[testcase_3] ld_bss_size: %d\n", ld_stats.ld_bss_size);
	XDEBUG_KPRINTF("[testcase_3] ld_data_addr: %x\n", ld_stats.ld_data_addr);
	XDEBUG_KPRINTF("[testcase_3] ld_data_size: %d\n", ld_stats.ld_data_size);

	XDEBUG_KPRINTF("[testcase_3] currpid: %d\n", currpid);
	XDEBUG_KPRINTF("[testcase_3] PROC 1 prio: %d\n", proctab[1].prprio);

	// create the proc
	resume(create(ld_stats._start_addr, INITSTK, INITPRIO, "testcase_3", 0, NULL));

	// unload
	/*unload(ld_stats.ld_text_addr);*/

	return 1;
}  
  

process testcase_3(void) {
  	resume(create(t3_proc1, 1024, 50, "t3_proc1", 0, NULL));

  	return OK;
}
