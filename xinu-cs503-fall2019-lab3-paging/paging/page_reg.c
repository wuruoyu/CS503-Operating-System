#include <xinu.h>

unsigned long tmp;

unsigned long read_cr3(void) {
    intmask mask;
    mask = disable();
    unsigned long local_tmp;

    asm("pushl %eax");
    asm("movl %cr3, %eax");
    asm("movl %eax, tmp");
    asm("popl %eax");

    local_tmp = tmp;

    restore(mask);

    return local_tmp;
}

void set_cr3(unsigned long n) {
    intmask mask;
    mask = disable();

    tmp = n;
    asm("pushl %eax");
    asm("movl tmp, %eax");		
    asm("movl %eax, %cr3");
    asm("popl %eax");

    restore(mask);
    return;
}

unsigned long read_cr0(void) {
    intmask mask;
    mask = disable();
    unsigned long local_tmp;

    asm("pushl %eax");
    asm("movl %cr0, %eax");
    asm("movl %eax, tmp");
    asm("popl %eax");

    local_tmp = tmp;

    restore(mask);

    return local_tmp;
}

void set_cr0(unsigned long n) {
    intmask mask;
    mask = disable();

    tmp = n;
    asm("pushl %eax");
    asm("movl tmp, %eax");		
    asm("movl %eax, %cr0");
    asm("popl %eax");

    restore(mask);
    return;
}
