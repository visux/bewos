#ifndef __PHISICMEM_H
#define __PHISICMEM_H

#define SIZE_PAGE 4096
#define MAX_ELEM_VETT 32768 //Il numero massimo di elementi della bitmap
#define KERNEL_MEM 4 //Memoria in MB riservata al kernel
#define KERNEL_MEM_PAGES	(KERNEL_MEM * 1024 * 1024) / SIZE_PAGE //Numero di pagine riservate al kernel
#define KERNEL_MEM_ELEMENTS	KERNEL_MEM_PAGES / 32 //Numero di elementi riservato al kernel nella bitmap

void size_mem(void);
void init_pages(void);
void* alloc_pages(unsigned int num);
void add_info_pages(unsigned int  address, unsigned int num);
void dealloc_pages(void* address);

extern unsigned int max_pages_intomem;
extern unsigned int real_elem_vett;
extern unsigned int total_mem; //Viene inizializzata nel file main.c
extern unsigned int status_kmalloc;

struct info_pages  //questa struttura contiene le informazioni delle pagine richieste che riguardano l' indirizzo di partenza e il numero di
				  //pagine allocate
{
 void* base_address;
 unsigned int num_pages;
 };
 
 //Questa sottostante e' una struttura ricorsiva (lista) caratterizzata da una defizione di un puntatore alla struttura nel quale e' definito.
 //Questo ci permette  facilmente di segnare le info delle pagine richieste
 
 // Ricorda: Le strutture ricorsive sono utilizzate per rappresentare quelle informazioni che:
 //  1. possono avere intrinsecamente una struttura molto complessa: alberi, grafi, diagrammi di flusso;
 //  2. la loro dimensione può variare molto spesso durante l'uso: liste, code, pile.

 //Noi la utilizziamo per quest' ultimo caso; infatti non sappiamo a priori quanto grande deve essere l' array vett_pages_info(510 e' soltanto
//una dimensione di default) quindi dobbiamo allocare la stessa struttura e falla puntare dal puntatore next di tipo mem_page_info. Quindi
// creando una lista ogni volta che ce ne bisogno possiamo allocare strutture di tale genere e utilizzare quindi gli  array info_pages(da 510
// elementi ciascuna) delle rispettive strutture allocate.

//esempio: se abbiamo bisogno di 1020 elementi dell' array vett_pages_info abbiamo bisogno di due strutture mem_pages_info(che vengono
//chiamate elementi della lista). Una struttura punta all' altra tramite  struct mem_page_info* next;

 struct mem_pages_info
 {
 struct info_pages vett_pages_info[510]; //vettore di struct info_pages
 unsigned int next_free_index;  //teniamo conto del prossimo indice del vettore sopra-stante libero
 struct mem_pages_info* next;  //punta al prossimo elemento(struttura) della lista
 };
 

extern struct mem_pages_info info_pages_alloc;  //questa e' la prima struttura che assegno alla lista 
extern struct mem_pages_info* list_info_pages; //dichiaro una lista di tipo mem_pages_info
extern struct mem_pages_info* tmp_list_info_pages; //utilizzo questa variabile per contenere temporaneamente l' indirizzo di un nuovo
												     //elemento creato
extern struct mem_pages_info* punt_first_struct_list;  //utilizzeremo questo puntatore per puntare sempre alla prima struttura della 
										          		 //lista

#endif
