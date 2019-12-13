#include <xinu.h>
#include <syscall_interface.h>

void t4_proc1() {
	int status;

    XDEBUG_KPRINTF("heap: %x\n", memlist);

	// hello elf
	char* hello_elf = apphello_contents;
	fileregister("/elf/hello.so", hello_elf, apphello_contents_size);

	// libmy elf
	char* libmy_elf = libmy_contents;
	fileregister("/so/libmy.so", libmy_elf, libmy_contents_size);

    // dlopen an invalid elf file

    // dlopen 
    void* handle;
    handle = dlopen("/so/libmy.so");

    // dlsym
    char* symbol_addr;
    symbol_addr = dlsym(handle, "mylib");
    XDEBUG_KPRINTF("[dlsym] mylib addr: %x\n", symbol_addr);

    // do that func
    /*resume(create(symbol_addr, INITSTK, INITPRIO, "testcase_4", 0, NULL));*/

    sleep(10);
    XDEBUG_KPRINTF("[testcase_4] second run\n");

    // automatic loading
	char* hello_dyn = apphello_dyn_loading_contents;
	fileregister("/so/hello_dyn.so", hello_dyn, apphello_dyn_loading_contents_size);
    
    struct load_t ld_stats;
    status = load("/so/hello_dyn.so", &ld_stats);
    resume(create(ld_stats._start_addr, INITSTK, INITPRIO, "testcase_4_dyn", 0, NULL));
    
    sleep(10);

    XDEBUG_KPRINTF("[testcase_4] ungister one\n");
    fileunregister("/elf/hello.so");
    resume(create(ld_stats._start_addr, INITSTK, INITPRIO, "testcase_4_dyn", 0, NULL));

    sleep(10);

    XDEBUG_KPRINTF("[testcase_4] ungister two\n");
    fileunregister("/so/libmy.so");
    resume(create(ld_stats._start_addr, INITSTK, INITPRIO, "testcase_4_dyn", 0, NULL));

    sleep(10);

    // dlclose
    dlclose(handle);

    XDEBUG_KPRINTF("heap: %x\n", memlist);

	return 1;
}  
  

process testcase_4(void) {
  	resume(create(t4_proc1, 1024, 50, "t4_proc1", 0, NULL));

  	return OK;
}
