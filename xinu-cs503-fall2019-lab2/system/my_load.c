/* load.c - load elf */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  load - load elf 
 *------------------------------------------------------------------------
 */

/* check that it has a valid elf header */
int is_image_valid(Elf32_Ehdr *hdr)
{
	char* h = (char*)hdr;
	if( h[0] == 0x7f && h[1] ==  0x45 && h[2] == 0x4c ) {
		return 1;
	}
	XDEBUG_KPRINTF("[loader] ELF file is invalid\n");
	return 0;
}

void* find_sym(const char* name, Elf32_Shdr* shdr, const char* strings, const char* src, char* dst)
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

void *image_load (char *elf_start, unsigned int size, struct load_t* ld_stats)
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

    bpid32 elfbufpool = mkbufpool(size, 1);
    exec = getbuf(elfbufpool);
  
    if(!exec) {
        XDEBUG_KPRINTF("image_load:: error allocating memory\n");
        return 0;
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
		    freebuf(exec);
                    return 0;
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
        /*if (shdr[i].sh_type == SHT_DYNSYM) {*/
            /*syms = (Elf32_Sym*)(elf_start + shdr[i].sh_offset);*/
            /*strings = elf_start + shdr[shdr[i].sh_link].sh_offset;*/
	    /*XDEBUG_KPRINTF("[loader] DYNSYM (syms: %x, strings: %x)", syms, strings);*/
            /*[>break;<]*/
        /*}*/
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

    /*for(i=0; i < hdr->e_shnum; ++i) {*/
        /*if (shdr[i].sh_type == SHT_REL) {*/
            /*relocate(shdr + i, syms, strings, elf_start, exec);*/
        /*}*/
    /*}*/

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
	}

	image_load(file, file_sz, ld_stats);

	fileclose(program_file_path);
	restore(mask);
}

int unload(void* ld_text_addr) {

	intmask mask;
	mask = disable();
	int i;

	for (i = 0; i < NLOAD; i ++) {
		if (loadtab[i].status == LOAD_OCCUPIED) {
			if (loadtab[i].ld_text_addr == ld_text_addr) {
				loadtab[i].status = LOAD_FREE;
				freebuf(loadtab[i].exec);
				XDEBUG_KPRINTF("[unloader] Gocha and Free!\n");
				break;
			}
		}
	}

	restore(mask);
}
