#include "include/syscall_interface.h"

extern int main();

void _start(){
      main();
}

int i;		//set global variable
void * handle;
int (*ptr)();
int data_section = 1;

int main(){
      int j=100;	//set local variable
      i= 1;
      j++;

      // getprio: 1 argument
      /*i = syscall_interface(2, 8, 0);*/
      /*XDEBUG_KPRINTF("prio: %d\n", i);*/

      i = syscall_interface(2, 8, 1);

      /*XDEBUG_KPRINTF("data section: %d\n", data_section);*/
      data_section += 1;

      /*syscall_interface(2,10,13); 	//same as kill(13);*/
      /*syscall_interface(2,10,14);	//same as kill(14);*/

      //uncomment below when you want to implement part 4
      /*
      handle = (void*)syscall_interface(2,0,"libmy.so");//same as dlopen("libmy.so");
      ptr=(void *)syscall_interface(3,2,handle,"mylib"); //same as dlsym(handle, "mylib");
      (*ptr)();
      */
      return 1;
}

