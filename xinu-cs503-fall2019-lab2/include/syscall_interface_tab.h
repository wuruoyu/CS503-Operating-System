//lab2 added

struct  syscall_interface_ent {
        char    *cname;
        syscall (*funcaddr)();
};

extern  const   struct  syscall_interface_ent  syscall_interface_tab[];

