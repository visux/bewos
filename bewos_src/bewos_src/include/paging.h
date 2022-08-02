#ifndef __IDT_H
#define __IDT_H


/*I bit da 0 a 11 delle entry della page dir e page table vengono utilizzati come flag */
/* I flag più importanti sono:


* P (present) flag, bit 0
	Indica se la pagina e' caricata in memoria (settato) o meno (non
	settato). Se si vuole acccedere a una pagina non in memoria, accade il solito
	e comune #PF (page-fault exception).

* R/W (Read/Write) flag, bit 1
	Indica se la pagina e' solamente readable (non settato) o anche
	writeable (settato). Questo flag interagisce coi flag U/S e WP nel CR0.

* U/S (User/Supervisor) flag, bit 2
	Indica il privilegio user/supervisor. Se non e' settato la pagina (o
	il gruppo di pagine) e' assegnata al livello supervisor, altrimenti al
	livello user. Tale flag interaisce con i flag R/W e WP in CR0.

* A (Accessed) flag, bit 5
	Indica se vi e' stato un accesso alla pagina o alla page table.

* D (Dirty) flag, bit 6
	Indica, se settato, che la pagina e' stata scritta.

* PS (Page Size) flag, bit 7
	Determina la grandezza della pagina (usato solo nelle entry della
	page-dir). Se e' spento, la size e' 4KB e la page dir entry punta a una page
	table. Se e' acceso, la size e' 4MB per l'indirizzamento a 32bit (e 2MB se e'
	attivato l'indirizzamento esteso) e la page dir entry punta a una pagina. Se
	tale entry punta a una page table, tutte le pagine associate con quella table
	saranno di 4KB.

* G (Global) flag, bit 8
	[Dai Pentium Pro] Indica una global page. Questo flag viene usato 
	specialmente per evitare che le pagine piu' usate siano cancellate dai TLB
	(Translation Lookaside Buffer - cache on-chip dove il processore conserva le pagine utilizzate piu' recentemente)
*/

#define PAGE_SIZE 4 //Dimensione in KB. Pagine da 4KB

#define PRESENT  1			// bit 0 (P) - 						In binario: 		000000001
#define READ_WRITE  2		// bit 1 (R/W) - 						In binario: 		000000010
#define SUPERVISOR  4		// bit 2 (U/S) - 						In binario: 		000000100
#define WRITE_THROUGH  8	// bit 3 (PWT) - 						In binario: 		000001000
#define CACHE_DISABLE  16	// bit 4 (PCD) - 						In binario: 		000010000
#define ACCESSED  32		// bit 5 (A) - 						In binario: 		000100000
#define DIRTY  64			// bit 6 (D) (Only for page table entries)		In binario:		001000000
#define PAGE_4MB  128		// bit 7 (PS) (Only for page directory entries)	In binario:		010000000
#define GLOBAL  256			// bit 8 (G)						        In binario:		100000000

#define KERNEL_PAGES_FLAG 	PRESENT | READ_WRITE //Flags che utilizzero' per le entry della pd e pt del kernel
#define USER_PAGES_FLAG 	PRESENT | READ_WRITE | SUPERVISOR //Flags che utilizzero' per le entry della pd e pt 

#include<string.h> //lo incluso per la define NULL

void enable_paging(void);
void init_paging(void);
void mappage(unsigned int virtual_address, void* real_address, unsigned int flags);
unsigned long return_virt_addr_pte(unsigned int n_pt, unsigned int pt_entry);
unsigned long return_virt_addr_pde(unsigned int pd_entry);
void edit_pde(unsigned int num_pde, void* address, unsigned int flags);
void edit_pte(unsigned int num_pde, unsigned int num_pte, void* address, unsigned int flags);
unsigned long read_pde(unsigned int num_pde);
unsigned long read_pte(unsigned int num_pde, unsigned int num_pte);
unsigned int find_first_pde_free(void);
void* find_first_pte_free(void);
void print_pagedir(int pd_entry);
int is_present(unsigned long entry);
void* make_virtual_address(unsigned int pde, unsigned int pte, unsigned int offset);

struct cur_paging {
unsigned int* cur_pagedir;
unsigned int index_pagedir;
unsigned int index_pagetable;
};

extern struct cur_paging cur_info_paging;

#endif
