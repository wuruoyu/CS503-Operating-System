#include <xinu.h>

const   struct  syscall_interface_ent  syscall_interface_tab[] = {
	{"dlopen",dlopen},
        {"dlclose",dlclose},
        {"dlsym",dlsym},
        {"close",close},
        {"control",control},
        {"freebuf",freebuf},
        {"freemem",freemem},
        {"getc",getc},
        {"getprio",getprio},
        {"init",init},
        {"kill",kill},
        {"mount",mount},
        {"open",open},
        {"ptcreate",ptcreate},
        {"ptdelete",ptdelete},
        {"ptinit",ptinit},
        {"ptreset",ptreset},
        {"ptsend",ptsend},
        {"read",read},
        {"seek",seek},
        {"semcount",semcount},
        {"semdelete",semdelete},
        {"semreset",semreset},
        {"send",send},
        {"signal",signal},
        {"signaln",signaln},
        {"sleep",sleep},
        {"sleepms",sleepms},
        {"suspend",suspend},
        {"suspend",suspend},
        {"wait",wait},
        {"write",write},
        {"yield",yield}

};

