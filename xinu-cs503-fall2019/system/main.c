/*  main.c  - main */

#include <xinu.h>

process	main(void)
{

	/* Run the Xinu shell */

	recvclr();
	pid32 pid = create(shell, 8192, PSSCHED, 50, "shell", 1, CONSOLE);
	kprintf("Spawning new shell with PID = %d\n", pid);
	resume(pid);

	/* Wait for shell to exit and recreate it */

	while (TRUE) {
		receive();
		sleepms(200);
		pid32 pid = create(shell, 4096, PSSCHED, 20, "shell", 1, CONSOLE);
		kprintf("Spawning new shell with PID = %d\n", pid);
		resume(pid);
	}
	return OK;
    
}
