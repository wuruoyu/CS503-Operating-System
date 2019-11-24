#include <xinu.h>

void pagehandler(void) {
    XDEBUG_KPRINTF("[pagehandler] page fault\n");
    return;
}
