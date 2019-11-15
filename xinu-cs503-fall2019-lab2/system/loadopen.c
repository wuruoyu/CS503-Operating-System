/* loadopen.c - */

#include <xinu.h>
	
static void* find_sym(const char* name, Elf32_Sym* syms, int sh_size, const char* strings, char* exec)
{
    int i;
    for (i = 0; i < sh_size / sizeof(Elf32_Sym); i ++) {
        if (strcmp(name, strings + syms[i].st_name) == 0) {
            return exec + syms[i].st_value;
        }
    }
    return NULL;
}

/* check that it has a valid elf header */
static int is_image_valid(Elf32_Ehdr *hdr) 
{
	char* h = (char*)hdr;
	if( h[0] == 0x7f && h[1] ==  0x45 && h[2] == 0x4c ) {
		return 1;
	}
	XDEBUG_KPRINTF("[dlopen] ELF file is invalid\n");
	return 0;
}

static syscall image_load (char *elf_start, unsigned int size)
{

    Elf32_Ehdr      *hdr     = NULL;
    Elf32_Phdr      *phdr    = NULL;
    Elf32_Shdr      *shdr    = NULL;
    Elf32_Sym       *sym_syms    = NULL;
    char            *sym_strings = NULL;
    int             sym_sh_size = 0;
    Elf32_Sym       *dyn_syms    = NULL;
    char            *dyn_strings = NULL;
    int             dyn_sh_size = 0;
    char            *start   = NULL;
    char            *taddr   = NULL;
    int i = 0;
    char *exec = NULL;

    hdr = (Elf32_Ehdr *) elf_start;
    if (is_image_valid(hdr) != 0) {
        XDEBUG_KPRINTF("[dlopen] ELF file is valid\n");
    }
    else {
        XDEBUG_KPRINTF("[dlopen] ELF file is invalid\n");
	    return SYSERR;
    }

    bpid32 elfbufpool = mkbufpool(size, 1);
    exec = getbuf(elfbufpool);
  
    if(!exec) {
        XDEBUG_KPRINTF("image_load:: error allocating memory\n");
        return SYSERR;
    }

    // Start with clean memory.
    memset(exec, 0x0, size);

    // iterate through program headers
    phdr = (Elf32_Phdr *)(elf_start + hdr->e_phoff);
    for (i = 0; i < hdr->e_phnum; ++i) {
            if (phdr[i].p_type != PT_LOAD) {
                    continue;
            }
            if (phdr[i].p_filesz > phdr[i].p_memsz) {
                    XDEBUG_KPRINTF("[dlopen] image_load:: p_filesz > p_memsz\n");
		    freebuf(exec);
                    return SYSERR;
            }
            if(!phdr[i].p_filesz) {
                    continue;
            }

            // p_filesz can be smaller than p_memsz,
            // the difference is zeroe'd out.
            start = elf_start + phdr[i].p_offset;
            taddr = phdr[i].p_vaddr + exec;
	    XDEBUG_KPRINTF("[dlopen] move %x -> %x (%d bytes, offset %x) \n", start, taddr, phdr[i].p_filesz, phdr[i].p_offset);
	    memcpy(taddr, start, phdr[i].p_filesz);
    }


    // retrieve string section table
    shdr = (Elf32_Shdr *)(elf_start + hdr->e_shoff);
    Elf32_Shdr* sh_strtab = &shdr[hdr->e_shstrndx];
    char* sh_strtab_p = elf_start + sh_strtab->sh_offset;

    // SYMTAB
    for (i = 0; i < hdr->e_shnum; ++i) {
	    if (shdr[i].sh_type == SHT_SYMTAB) {
            sym_syms = (Elf32_Sym*)(elf_start + shdr[i].sh_offset);
            sym_strings = elf_start + shdr[shdr[i].sh_link].sh_offset;
            sym_sh_size = shdr[i].sh_size;
	        XDEBUG_KPRINTF("[dlopen] SYMTAB (sym_syms: %x, sym_strings: %x)\n", sym_syms, sym_strings + shdr[i].sh_name);
            /*char* entry = find_sym("_start", shdr + i, sym_strings, elf_start, exec);*/
            char* entry = find_sym("_start", sym_syms, sym_sh_size, sym_strings, exec);
            /*XDEBUG_KPRINTF("[dlopen] entry: %x\n", entry);*/
            /*XDEBUG_KPRINTF("[dlopen] sym_syms: %x\n", sym_syms);*/
            /*XDEBUG_KPRINTF("[dlopen] sym_sh_size: %d\n", sym_sh_size);*/
            /*XDEBUG_KPRINTF("[dlopen] sym_strings: %x\n", sym_strings);*/
            /*XDEBUG_KPRINTF("[dlopen] exec: %x\n", exec);*/
	    }
		/*else if (shdr[i].sh_type == SHT_PROGBITS) {*/
			/*if (strcmp(".data", sh_strtab_p + shdr[i].sh_name) == 0) {*/
			/*}*/
			/*if (strcmp(".text", sh_strtab_p + shdr[i].sh_name) == 0) {*/
			/*}*/
		/*}*/
		/*else if (shdr[i].sh_type == SHT_NOBITS) {*/
			/*if (!shdr[i].sh_size) {*/
				/*continue;*/
			/*}*/
		/*}*/
    }

    // DYNTAB
    for (i = 0; i < hdr->e_shnum; ++i) {
        if (shdr[i].sh_type == SHT_DYNSYM) {
            dyn_syms = (Elf32_Sym*)(elf_start + shdr[i].sh_offset);
            dyn_strings = elf_start + shdr[shdr[i].sh_link].sh_offset;
            dyn_sh_size = shdr[i].sh_size;
            XDEBUG_KPRINTF("[dlopen] DYNSYM (dyn_syms: %x, dyn_strings: %x)\n", dyn_syms, dyn_strings);
        }
        if (shdr[i].sh_type == SHT_REL) {
            /*relocate(shdr + i, dyn_syms, dyn_strings, elf_start, exec);*/
            XDEBUG_KPRINTF("[dlopen] Find REL, but we ignore\n");
        }
    }

    // construct handle
    struct dl_handle_t ret_handle;
    ret_handle.status = HANDLE_OPEN;
    ret_handle.pid = currpid;
    ret_handle.elf_start = elf_start;
    ret_handle.exec = exec;
    ret_handle.sym_syms = sym_syms;
    ret_handle.sym_strings = sym_strings;
    ret_handle.sym_sh_size = sym_sh_size;
    ret_handle.dyn_syms = dyn_syms;
    ret_handle.dyn_strings = dyn_strings;
    ret_handle.dyn_sh_size = dyn_sh_size;


    // register the handle
    for (i = 0; i < NHANDLE; i++) {
        if (handletab[i].status == HANDLE_CLOSE) {
            handletab[i] = ret_handle;
            break;
        }
    }
    if (i == NHANDLE) {
        XDEBUG_KPRINTF("[dlopen] no available handle slot\n");
        return SYSERR;
    }

    return i;
}


char* fileopen_load(char* path, int* file_size_out)
{
	int32 i;

	// check if file path has been registered
	for (i = 0; i < NFILE; i ++) {
		if (filetab[i].filestate == FILE_OCCUPIED) {
			if (strcmp(filetab[i].filepath, path) == 0) {
				*file_size_out = filetab[i].filesize; 
				return filetab[i].filecontent;
			}
		}
	}	
	return SYSERR;
}

syscall loadopen(char* library_file_path, int id)
{
	int32 i;

	// file not registered
	int file_size = 0;
	char* file = fileopen_load(library_file_path, &file_size);
	if ((char*)SYSERR == file) {
		XDEBUG_KPRINTF("[loadopen] file not exist\n");
		return SYSERR;
	}

    int handle = image_load(file, file_size);
	if (handle == SYSERR) {
		return SYSERR;
	}

    struct load_t ld_stats;
    ld_stats.status = LOAD_FREE;
    ld_stats.automatic_load_state = AUTOMATIC_ON;
    ld_stats.load_by = id;

    // register in the loadtab
    for (i = 0; i < NLOAD; i ++) {
	    if (loadtab[i].status == LOAD_FREE) {
		    loadtab[i] = ld_stats;
		    break;
	    }
    }

	return handle;
}
