
struct load_t {
	void * _start_addr;
	void * ld_text_addr;
	int ld_text_size;
	void* ld_bss_addr;
	int ld_bss_size;
	void* ld_data_addr;
	int ld_data_size;
	char* exec;
	int size;
	int status;
};

//extern int32 elfbufpool;

#define NLOAD 		10
#define LOAD_OCCUPIED 	0
#define LOAD_FREE	1 

extern struct load_t loadtab[];
