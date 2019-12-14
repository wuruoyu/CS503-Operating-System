#define NHANDLE 20

#define HANDLE_OPEN 1
#define HANDLE_CLOSE 0

extern syscall dlsym (void *, const char*);

extern syscall dlopen(char *);

extern syscall dlclose(void *);

extern syscall loadopen(char*, int);

struct dl_handle_t {
    int     status;
    pid32   pid;
    char*   elf_start;
    char*   exec;
    char*   sym_syms;
    char*   sym_strings;
    int     sym_sh_size;
    char*   dyn_syms;
    char*   dyn_strings;
    int     dyn_sh_size;
};

extern struct dl_handle_t handletab[];
