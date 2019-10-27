//lab2 added
int retval;
int *args;
int orinargs;
int test;
int syscall_interface(int nargs, ... ){
	
	orinargs = nargs;
	args = (int *)(&nargs +1);

	args = args + 5  - 1;
	
	//push five arguments and number of nargs to stack
	//Note that the address of argument in your handler function may not be exactly same as the address being "pushed" here
	//You may want to print the address of esp to understand where these data are stored on the stack
	asm(
	    "movl %1,%%eax;"
            "pushl %%eax;"
	    "movl %2,%%eax;"
            "pushl %%eax;"
            "movl %3,%%eax;"
            "pushl %%eax;"
            "movl %4,%%eax;"
            "pushl %%eax;"
            "movl %5,%%eax;"
            "pushl %%eax;"
	    "movl %6, %%eax;"
	    "pushl %%eax;"
	    "int $80;"
	    "movl %%eax, %0;"
	    "popl %%eax;"
	    "popl %%eax;"
       	    "popl %%eax;"
            "popl %%eax;"
            "popl %%eax;"
            "popl %%eax;"
	    : "=r"(retval)
	    :"g"(*args), "g"(*(args-1)),"g"(*(args-2)),"g"(*(args-3)),"g"(*(args-4)),"g" (orinargs)
	    : "%eax"
	   );

	return retval;
}
