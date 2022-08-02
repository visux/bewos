/* Black & White Operating System
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details (in the COPYING file).
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/******************************************INFO**********************************************
* Coders: Lord Rust
* Descrizione: Strutture per l'implementazione dei binari ELF
*********************************************************************************************/

#ifndef __ELF_H
#define __ELF_H

// Mi sono permesso di rippare il rippabile da /usr/include/linux/elf.h :)

typedef __u32 Elf32_Addr;
typedef __u16 Elf32_Half;
typedef __u32 Elf32_Off;
typedef __s32 Elf32_Sword;
typedef __u32 Elf32_Word;

typedef struct dynamic {
	Elf32_Sword d_tag;
	union {
		Elf32_Sword d_val;
		Elf32_Addr  d_ptr;
	} d_un;
} Elf32_Dyn;

typedef struct e32rel {
	Elf32_Addr	r_offset;
	Elf32_Word	r_info;
} Elf32_Rel;

typedef struct e32rel {
	Elf32_Addr	r_offset;
	Elf32_Word	r_info;
	Elf32_Sword	r_addend;
} Elf32_Rela;

typedef struct e32sym {
	Elf32_Word	st_name;
	Elf32_Addr	st_value;
	Elf32_Word	st_size;
	unsigned char	st_info;
	unsigned char	st_other;
	Elf32_Half	st_shndx;
} Elf32_Sym;

#define EI_NIDENT 16

typedef struct e32ehdr {
	unsigned char	e_ident[ EI_NIDENT ];
	Elf32_Half	e_type;
	Elf32_Half	e_machine;
	Elf32_Word	e_version;
	Elf32_Addr	e_entry;
	Elf32_Off	e_phoff;
	Elf32_Off	e_shoff;
	Elf32_Word	e_flags;
	Elf32_Half	e_ehsize;
	Elf32_Half	e_phentsize;
	Elf32_Half	e_phnum;
	Elf32_Half	e_shentsize;
	Elf32_Half	e_shnum;
	Elf32_Half	e_shstrndx;
} Elf32_Ehdr;

typedef struct e32phdr {
	Elf32_Word	p_type;
	Elf32_Off	p_offset;
	Elf32_Addr	p_vaddr;
	Elf32_Addr	p_paddr;
	Elf32_Word	p_filesz;
	Elf32_Word	p_memsz;
	Elf32_Word	p_flags;
	Elf32_Word	p_align;
} Elf32_Phdr;

typedef struct e32shdr {
	Elf32_Word	sh_name;
	Elf32_Word	sh_type;
	Elf32_Word	sh_flags;
	Elf32_Addr	sh_addr;
	Elf32_Off	sh_offset;
	Elf32_Word	sh_size;
	Elf32_Word	sh_link;
	Elf32_Word	sh_info;
	Elf32_Word	sh_addralign;
	Elf32_Word	sh_entsize;
} Elf32_Shdr;

/* Note header in a PT_NOTE section */
typedef struct elf32_note {
	  Elf32_Word    n_namesz;       /* Name size */
	  Elf32_Word    n_descsz;       /* Content size */
	  Elf32_Word    n_type;         /* Content type */
} Elf32_Nhdr;

//
// Seguono le definizioni relative ai vari header.
//

#define EI_MAG0         0               /* questi valori sono indici di e_ident[] */
#define EI_MAG1         1
#define EI_MAG2         2
#define EI_MAG3         3
#define EI_CLASS        4
#define EI_DATA         5
#define EI_VERSION      6
#define EI_OSABI        7
#define EI_PAD          8

// Definizione del magic number ELF; un file ELF deve iniziare con "\177ELF"
#define ELFMAG0         0x7f            /* EI_MAG */
#define ELFMAG1         'E'
#define ELFMAG2         'L'
#define ELFMAG3         'F'
#define ELFMAG          "\177ELF"
#define SELFMAG         4

#define ELFCLASSNONE    0               /* EI_CLASS */
#define ELFCLASS32      1
#define ELFCLASS64      2
#define ELFCLASSNUM     3

#define ELFDATANONE     0               /* e_ident[EI_DATA] */
#define ELFDATA2LSB     1
#define ELFDATA2MSB     2

#define EV_NONE         0               /* e_version, EI_VERSION */
#define EV_CURRENT      1
#define EV_NUM          2

#define ELFOSABI_NONE   0
//#define ELFOSABI_LINUX  3		/* uhmm :P */

#ifndef ELF_OSABI
#define ELF_OSABI ELFOSABI_NONE
#endif

/* Note usate negli elf di tipo ET_CORE */
#define NT_PRSTATUS     1
#define NT_PRFPREG      2
#define NT_PRPSINFO     3
#define NT_TASKSTRUCT   4
#define NT_AUXV         6
#define NT_PRXFPREG     0x46e62b7f      /* copied from gdb5.1/include/elf/common.h */

// Definizioni relative alla Program Header table
#define PT_NULL    0
#define PT_LOAD    1
#define PT_DYNAMIC 2
#define PT_INTERP  3
#define PT_NOTE    4
#define PT_SHLIB   5
#define PT_PHDR    6
#define PT_TLS     7               /* Thread local storage segment */

// Definizione dei tipi di file ELF
#define ET_NONE   0		// nessun tipo
#define ET_REL    1		// rilocabile (file .o)
#define ET_EXEC   2		// eseguibile
#define ET_DYN    3		// dinamico (.so)
#define ET_CORE   4		// core

//
// Segue lista del tipo di macchine rippata pari pari.
//

#define EM_NONE  0
#define EM_M32   1
#define EM_SPARC 2
#define EM_386   3
#define EM_68K   4
#define EM_88K   5
#define EM_486   6   /* Perhaps disused */
#define EM_860   7

#define EM_MIPS         8       /* MIPS R3000 (officially, big-endian only) */

#define EM_MIPS_RS4_BE 10       /* MIPS R4000 big-endian */

#define EM_PARISC      15       /* HPPA */

#define EM_SPARC32PLUS 18       /* Sun's "v8plus" */

#define EM_PPC         20       /* PowerPC */
#define EM_PPC64       21       /* PowerPC64 */

#define EM_SH          42       /* SuperH */
#define EM_SPARCV9     43       /* SPARC v9 64-bit */

#define EM_IA_64        50      /* HP/Intel IA-64 */

#define EM_X86_64       62      /* AMD x86-64 */

#define EM_S390         22      /* IBM S/390 */

#define EM_CRIS         76      /* Axis Communications 32-bit embedded processor */

#define EM_V850         87      /* NEC v850 */

#define EM_M32R         88      /* Renesas M32R */

#define EM_H8_300       46      /* Renesas H8/300,300H,H8S */

/*
 *  * This is an interim value that we will use until the committee comes
 *  * up with a final number.
 *  */
#define EM_ALPHA        0x9026
/* Bogus old v850 magic number, used by old tools.  */
#define EM_CYGNUS_V850  0x9080

/* Bogus old m32r magic number, used by old tools.  */
#define EM_CYGNUS_M32R  0x9041

/*
 *  * This is the old interim value for S/390 architecture
 *  */
#define EM_S390_OLD     0xA390

#define EM_FRV          0x5441          /* Fujitsu FR-V */

// Informazioni relative alla parte dinamica dei file elf

#define DT_NULL         0
#define DT_NEEDED       1
#define DT_PLTRELSZ     2
#define DT_PLTGOT       3
#define DT_HASH         4
#define DT_STRTAB       5
#define DT_SYMTAB       6
#define DT_RELA         7
#define DT_RELASZ       8
#define DT_RELAENT      9
#define DT_STRSZ        10
#define DT_SYMENT       11
#define DT_INIT         12
#define DT_FINI         13
#define DT_SONAME       14
#define DT_RPATH        15
#define DT_SYMBOLIC     16
#define DT_REL          17
#define DT_RELSZ        18
#define DT_RELENT       19
#define DT_PLTREL       20
#define DT_PLTREL       20
#define DT_DEBUG        21
#define DT_TEXTREL      22
#define DT_JMPREL       23

// Info relative alla Symbol Table

#define STB_LOCAL  0
#define STB_GLOBAL 1
#define STB_WEAK   2

#define STT_NOTYPE  0
#define STT_OBJECT  1
#define STT_FUNC    2
#define STT_SECTION 3
#define STT_FILE    4

// [TODO] Aggiungere le macro [/TODO]

// Definizioni relative ai vettori ausiliari

#define AT_NULL   0     /* end of vector */
#define AT_IGNORE 1     /* entry should be ignored */
#define AT_EXECFD 2     /* file descriptor of program */
#define AT_PHDR   3     /* program headers for program */
#define AT_PHENT  4     /* size of program header entry */
#define AT_PHNUM  5     /* number of program headers */
#define AT_PAGESZ 6     /* system page size */
#define AT_BASE   7     /* base address of interpreter */
#define AT_FLAGS  8     /* flags */
#define AT_ENTRY  9     /* entry point of program */
#define AT_NOTELF 10    /* program is not ELF */
#define AT_UID    11    /* real uid */
#define AT_EUID   12    /* effective uid */
#define AT_GID    13    /* real gid */
#define AT_EGID   14    /* effective gid */
#define AT_PLATFORM 15  /* string identifying CPU for optimizations */
#define AT_HWCAP  16    /* arch dependent hints at CPU capabilities */
#define AT_CLKTCK 17    /* frequency at which times() increments */
#define AT_SECURE 23   /* secure mode boolean */

// Costanti che definiscono i permessi di un segmento (lettura, scrittura, esecuzione)

#define PF_R            0x4
#define PF_W            0x2
#define PF_X            0x1

// Tipologie di section header

#define SHT_NULL        0
#define SHT_PROGBITS    1
#define SHT_SYMTAB      2
#define SHT_STRTAB      3
#define SHT_RELA        4
#define SHT_HASH        5
#define SHT_DYNAMIC     6
#define SHT_NOTE        7
#define SHT_NOBITS      8
#define SHT_REL         9
#define SHT_SHLIB       10
#define SHT_DYNSYM      11
#define SHT_NUM         12

// Flag di permessi delle sezioni (lettura, scrittura, esecuzione)

#define SHF_WRITE       0x1
#define SHF_ALLOC       0x2
#define SHF_EXECINSTR   0x4

#define SHN_UNDEF	0x0
#define SHN_LORESERVE	0xff00
#define SHN_LOPROC	0xff00
#define SHN_HIPROC	0xff1f
#define SHN_ABS		0xfff1
#define SHN_COMMON	0xfff2
#define SHN_HIRESERVE	0xffff

void elfld_mmap(void* ptr); // Carica un file ELF precedentemente mmappato all'indirizzo ptr

#endif /* __ELF_H */
