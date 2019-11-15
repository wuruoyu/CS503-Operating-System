#include "include/syscall_interface.h"

int test;
int i;
int mylib(){
      int j=100;
      i= 1;
      j++;

      /*syscall_interface(2,10,20); //same as kill(20)*/
      /*syscall_interface(2,10,21); //same as kill(21)*/
      
      int ret;
      ret = syscall_interface(2, 26, 2);

      return 1;
}


