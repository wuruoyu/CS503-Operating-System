#include "include/syscall_interface.h"

extern int main();

void _start(){
      main();
}

int i;		//set global variable
void * handle;
int (*ptr)();

int main(){
      int j=100;	//set local variable
      i= 1;
      j++;

      syscall_interface(2,10,13); 	//same as kill(13);
      syscall_interface(2,10,14);	//same as kill(14);
      //uncomment below when you want to implement part 4
      /*
      handle = (void*)syscall_interface(2,0,"libmy.so");//same as dlopen("libmy.so");
      ptr=(void *)syscall_interface(3,2,handle,"mylib"); //same as dlsym(handle, "mylib");
      (*ptr)();
      */
      return 1;
}

