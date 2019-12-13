/* load.c - load elf */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  load - load elf 
 *------------------------------------------------------------------------
 */

int loadtab_id = 0;

static void *resolve(const char* sym)
{
    XDEBUG_KPRINTF("[loader] resolve - sym: %s\n", sym);
    // go through all the files
    int i;
    char* file;
    for (i = 0; i < NFILE; i++) {
        if (filetab[i].filestate == FILE_OCCUPIED) {
            void* handle = loadopen(filetab[i].filepath, loadtab_id);
            if ((syscall)handle == SYSERR) {
                XDEBUG_KPRINTF("[loader] resolve - load fail\n");
                continue;
            }
            else {
                void* ret_dlsym = dlsym(handle, sym);
                if (ret_dlsym == (void*)SYSERR) {
                    XDEBUG_KPRINTF("[loader] resolve - not find sym\n");
                    continue;
                }
                else {
                    XDEBUG_KPRINTF("[loader] resolve - find sym\n");
                    return ret_dlsym;
                }
            }
        }
    }
    XDEBUG_KPRINTF("[loader] resolve - we can not resolve symbol\n");
    return SYSERR;
}

static syscall relocate(Elf32_Shdr* shdr, const Elf32_Sym* syms, const char* strings, const char* src, char* dst)
{
    Elf32_Rel* rel = (Elf32_Rel*)(src + shdr->sh_offset);
    int j;
    /*XDEBUG_KPRINTF("[loader] relocate - num sym: %d\n", shdr->sh_size / sizeof(Elf32_Rel));*/
    for(j = 0; j < shdr->sh_size / sizeof(Elf32_Rel); j += 1) {
        const char* sym = strings + syms[ELF32_R_SYM(rel[j].r_info)].st_name;
        /*XDEBUG_KPRINTF("[loader] relocate - sym: %s\n", sym);*/
        Elf32_Word ret_resolve;
        switch(ELF32_R_TYPE(rel[j].r_info)) {
            case R_386_JMP_SLOT:
            case R_386_GLOB_DAT:
                ret_resolve = (Elf32_Word)resolve(sym);
                if ((syscall)ret_resolve == SYSERR) {
                    return SYSERR;
                }
                *(Elf32_Word*)(dst + rel[j].r_offset) = ret_resolve;
                break;
        }
    }
}

/* check that it has a valid elf header */
static int is_image_valid(Elf32_Ehdr *hdr)
{
	char* h = (char*)hdr;
	if( h[0] == 0x7f && h[1] ==  0x45 && h[2] == 0x4c ) {
		return 1;
	}
	XDEBUG_KPRINTF("[loader] ELF file is invalid\n");
	return 0;
}

static void* find_sym(const char* name, Elf32_Shdr* shdr, const char* strings, const char* src, char* dst)
{
    Elf32_Sym* syms = (Elf32_Sym*)(src + shdr->sh_offset);
    int i;
    for (i = 0; i < shdr->sh_size / sizeof(Elf32_Sym); i ++) {
        if (strcmp(name, strings + syms[i].st_name) == 0) {
            return dst + syms[i].st_value;
        }
    }
    return NULL;
}

static void *image_load (char *elf_start, unsigned int size, struct load_t* ld_stats)
{

    Elf32_Ehdr      *hdr     = NULL;
    Elf32_Phdr      *phdr    = NULL;
    Elf32_Shdr      *shdr    = NULL;
    Elf32_Sym       *syms    = NULL;
    char            *strings = NULL;
    char            *start   = NULL;
    char            *taddr   = NULL;
    void            *entry   = NULL;
    int i = 0;
    char *exec = NULL;

    hdr = (Elf32_Ehdr *) elf_start;
    if (is_image_valid(hdr) != 0) {
        XDEBUG_KPRINTF("[loader] ELF file is valid\n");
    }
    else {
        XDEBUG_KPRINTF("[loader] ELF file is invalid\n");
	    return SYSERR;
    }

    /*bpid32 elfbufpool = mkbufpool(size, 1);*/
    /*exec = getbuf(elfbufpool);*/

    exec = getmem(size);
  
    if(!exec) {
        XDEBUG_KPRINTF("image_load:: error allocating memory\n");
        return SYSERR;
    }

    // Start with clean memory.
    memset(exec,0x0,size);

    /*XDEBUG_KPRINTF("[loader] hdr->e_phnum: %d\n", hdr->e_phnum);*/
    /*XDEBUG_KPRINTF("[loader] hdr->e_phoff: %d\n", hdr->e_phoff);*/
    /*XDEBUG_KPRINTF("[loader] hdr->e_shnum: %d\n", hdr->e_shnum);*/
    /*XDEBUG_KPRINTF("[loader] hdr->e_shoff: %d\n", hdr->e_shoff);*/

    // iterate through program headers
    phdr = (Elf32_Phdr *)(elf_start + hdr->e_phoff);
    for (i = 0; i < hdr->e_phnum; ++i) {
            if (phdr[i].p_type != PT_LOAD) {
                    continue;
            }
            if (phdr[i].p_filesz > phdr[i].p_memsz) {
                    XDEBUG_KPRINTF("[loader] image_load:: p_filesz > p_memsz\n");
		    freemem(exec, size);
                    return SYSERR;
            }
            if(!phdr[i].p_filesz) {
                    continue;
            }

            // p_filesz can be smaller than p_memsz,
            // the difference is zeroe'd out.
            start = elf_start + phdr[i].p_offset;
            taddr = phdr[i].p_vaddr + exec;
	    XDEBUG_KPRINTF("[loader] move %x -> %x (%d bytes, offset %x) \n", start, taddr, phdr[i].p_filesz, phdr[i].p_offset);
	    memcpy(taddr, start, phdr[i].p_filesz);

            /*if(!(phdr[i].p_flags & PF_W)) {*/
                    /*// Read-only.*/
                    /*mprotect((unsigned char *) taddr,*/
                              /*phdr[i].p_memsz,*/
                              /*PROT_READ);*/
            /*}*/

            /*if(phdr[i].p_flags & PF_X) {*/
                    /*// Executable.*/
                    /*mprotect((unsigned char *) taddr,*/
                            /*phdr[i].p_memsz,*/
                            /*PROT_EXEC);*/
            /*}*/
    }


    // retrieve string section table
    shdr = (Elf32_Shdr *)(elf_start + hdr->e_shoff);
    Elf32_Shdr* sh_strtab = &shdr[hdr->e_shstrndx];
    char* sh_strtab_p = elf_start + sh_strtab->sh_offset;

    // iterate through section headers
    for (i = 0; i < hdr->e_shnum; ++i) {
        if (shdr[i].sh_type == SHT_SYMTAB) {
                syms = (Elf32_Sym*)(elf_start + shdr[i].sh_offset);
                strings = elf_start + shdr[shdr[i].sh_link].sh_offset;
            XDEBUG_KPRINTF("[loader] SYMTAB (syms: %x, strings: %x)\n", syms, strings + shdr[i].sh_name);
                entry = find_sym("_start", shdr + i, strings, elf_start, exec);
                ld_stats->_start_addr = entry;
        }
        else if (shdr[i].sh_type == SHT_PROGBITS) {
            if (strcmp(".data", sh_strtab_p + shdr[i].sh_name) == 0) {
                ld_stats->ld_data_addr = shdr[i].sh_addr + exec;
                ld_stats->ld_data_size = shdr[i].sh_size;
            }
            if (strcmp(".text", sh_strtab_p + shdr[i].sh_name) == 0) {
                ld_stats->ld_text_addr = shdr[i].sh_addr + exec;
                ld_stats->ld_text_size = shdr[i].sh_size;
            }
        }
        else if (shdr[i].sh_type == SHT_NOBITS) {
            if (!shdr[i].sh_size) {
                continue;
            }
            ld_stats->ld_bss_addr = shdr[i].sh_addr + exec;
            ld_stats->ld_bss_size = shdr[i].sh_size;
        }
    }
    
    // DYNMAIC
    for(i=0; i < hdr->e_shnum; ++i) {
        if (shdr[i].sh_type == SHT_DYNSYM) {
            syms = (Elf32_Sym*)(elf_start + shdr[i].sh_offset);
            strings = elf_start + shdr[shdr[i].sh_link].sh_offset;
            /*entry = find_sym("main", shdr + i, strings, elf_start, exec);*/
            break;
        }
    }

    // relocation table
    for(i = 0; i < hdr->e_shnum; ++i) {
        if (shdr[i].sh_type == SHT_REL) {
            if (relocate(shdr + i, syms, strings, elf_start, exec) == SYSERR) {
                return SYSERR;
            }
        }
    }

    XDEBUG_KPRINTF("[loader] entry: %x\n", entry);

    ld_stats->exec = exec;
    ld_stats->size = size;
    ld_stats->status = LOAD_OCCUPIED;

    // register in the loadtab
    for (i = 0; i < NLOAD; i ++) {
	    if (loadtab[i].status == LOAD_FREE) {
		    loadtab[i] = *ld_stats;
		    break;
	    }
    }
    loadtab_id = i;

    XDEBUG_KPRINTF("[loader] currpid: %d\n", currpid);

    return entry;
}

syscall load(char* program_file_path, struct load_t* ld_stats)
{
	intmask mask;
	mask = disable();

	int file_sz = 0;
	char* file = fileopen(program_file_path, &file_sz);
	if (file == (char*)SYSERR) {
		XDEBUG_KPRINTF("[load] file open error\n");
        restore(mask);
        return SYSERR;
	}

	if (image_load(file, file_sz, ld_stats) == (void*)SYSERR) {
		XDEBUG_KPRINTF("[loader] load error\n");
        restore(mask);
		return SYSERR;
	}

	fileclose(program_file_path);
	restore(mask);
    return OK;
}

