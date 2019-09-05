/*  main.c  - main */

#include <xinu.h>

process	main(void)
{

	/* Run the Xinu shell */

	recvclr();
	pid32 pid = create(shell, 8192, 50, "shell", 1, CONSOLE);
	kprintf("\n\nSpawning new shell with PID = %d\n\n", pid);
	resume(pid);

	/* Wait for shell to exit and recreate it */

	while (TRUE) {
		receive();
		sleepms(200);
		pid32 pid = create(shell, 4096, 20, "shell", 1, CONSOLE);
		kprintf("\n\nSpawning new shell with PID = %d\n\n", pid);
		resume(pid);
	}
	return OK;
    
}
