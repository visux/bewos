/* I campi di una entry della GDT sono i seguenti */
/*
* Segment Limit field, bit 0 --> 15 (1a dw) + 16 --> 19 (2a dw)
Indica la grandezza del segmento (il processore mette insieme i 2 campi per formare un valore di 20 bit). Se il flag G di granularita' non e' settato, la grandezza puo' variare a 1 byte a 1MB, se e' settato puo' variare da 4 KB a 4GB con incrementi di 4KB alla volta.

* Base Address field, bit 16 --> 31 (1a dw) + 0 --> 7 (2a dw) + 24 --> 31 (2a dw)
Indica la posizione del byte 0 del segmento all'interno del linear address space di 4GB (il processore mette insieme i 3 campi per fromare un valore a 32bit).

* Type field, bit 8 --> 11 (2a dw)
Indica il tipo di segmento, i tipi di accesso su di esso e la sua direzione di sviluppo.

* S (descriptor type) flag, bit 12 (2a dw)
Indica se abbiamo a che fare con un system segment (S non e' acceso) e un code / data segment (S e' acceso) 

*DPL (descriptor privilege level) field, bit 13 --> 14 (2a dw)
Altro campo importante per noi: indica il livello di privilegio del segmento (4 valori da 0 a 3).

* P (segment present) flag, bit 15 (2a dw)
Indica se il segmento e' presente (acceso) o no (spento) in memoria. Se, da spento, si carica un segment register con un selettore che punta ad esso, viene generata un'exception segment-not-present (#NP). Inoltre, se tale flag non e' settato, il formato del descrittore stesso cambia.

* D/B (default operation size/default stack pointer size and/or upper bound) flag, bit 22 (2a dw)
Tale flag ha differenti funzioni (che non analizzeremo) a seconda che il segmento sia un code segment, expand-down data segment e stack segment.(Tale bit fa parte dei nybble flags)

* G (granularity) flag, bit 23 (2a dw)
Determina la scala di aumento del segment limit. Se non e' settato il limite e' interpretato in unita' di byte, in caso contrario in unita' di 4KB.(Tale bit fa parte dei nybble flags). Grazie a tale bit e' possibile specificare segmenti con Limit di 4Gb. Infatti ricordando che il Limit e' di 20 bit il processore moltiplica il valore per 4096(4kb), 2^20 * 4096 = 4GB :)) . Se tale bit e' settato a 0 e' possibile specificare una grandezza pari solo a 2^20 :((

Il bit 20 della seconda dw e' disponibile per il software, il bit 21 e' riservato e dovrebbe esser sempre lasciato a 0.
Nel caso in cui il flag S sia settato, il descriptor puo' riferirsi a segmento di codice o dati. E' il bit 11 della seconda dw del descriptor (il bit piu' significativo del type field) a determinare quando il descriptor stesso sia per segmenti di codice o dati.
Per i data segment, i 3 bit meno significativi indicano quando il segmento sia accessed (A), se si puo' scrivere su di esso (W) e la direzione di espansione (E).
Per i code segment, tali 3 bit stanno ad indicare quando il segmento e' accessed (A), se lo si puo' leggere (R) e se e' conforming (C).

*/


#ifndef __GDT_H
#define __GDT_H

//Numero massimo di entry nella GDT (*G*lobal *D*escriptor *T*able).
#define GDT_MAX_ENTRY 	8192
//Numero di entry che effettivamente utilizzo
#define USE_N_ENTRY 6
//Dimensione in byte di una entry della GDT
#define GDT_ENTRY_DIM	8





//Struttura della TSS. Una TSS e' di 103 byte + la bitmap_io_permission
struct x86_TSS
{
	unsigned int	link;
	unsigned int	ESP0;
	unsigned short	SS0, __ss0h;
	unsigned int	esp1;
	unsigned short	ss1, __ss1h;
	unsigned int	esp2;
	unsigned short	ss2, __ss2h;
	unsigned int	cr3;
	unsigned int	eip;
	unsigned int	eflags;
	unsigned int	eax, ecx, edx, ebx;
	unsigned int	esp;
	unsigned int	ebp;
	unsigned int	esi;
	unsigned int	edi;
	unsigned short	es, __esh;
	unsigned short	cs, __csh;
	unsigned short	SS, __ssh;
	unsigned short	ds, __dsh;
	unsigned short	fs, __fsh;
	unsigned short	gs, __gsh;
	unsigned short	ldtr, __ldtrh;
	unsigned short	trace, IO_bitmap_offset;
	
	unsigned int	bitmap_io_permission[8192/32];
};


extern struct x86_TSS *TSS;


struct gdt_descriptor {
  /*Byte 0 e 1*/
   unsigned short int limit0_15;		/* limit 0_15    */
   
   /*Byte 2 e 3*/
   unsigned short int base0_15;      	/* base  0_15    */
   
   /*Byte 4*/
   unsigned char base16_23;       		/* base  16_23   */
   
   /*Byte 5*/
   unsigned char access;            		/* access byte    */
   
   /*Byte 6*/
   unsigned int limit16_19:4;         		/* limit 16_19   */
   unsigned int flags_nybble:4;   		/* flags_nybble  */

   
   /*Byte 7*/
   unsigned char base24_31;    		/* base 24_31    */
   
} __attribute__ ((packed));

/*Il Byte "Access"(Byte 5) e' diviso nei seguenti bit:
Bit 7: Present
Bit 6-5: DPL(Descriptor Privilege Level)
Bit 4: S(system segment). Settato a 1 se non e' un segmento di sistema, ma un code/data segment

_TYPE_
Bit 3: Executable
Bit 2: Expansion direction/conforming
Bit 1: Writable / Readable
Bit 0: Accessed


I 4 Flags Nybble(nel Byte 6) sono i seguenti:
Bit 7: Granularity
Bit 6: Default size
Bit 5: 0
Bit 4: AVL
*/

//extern struct gdt_descriptor GDT[USE_N_ENTRY];
extern struct gdt_descriptor *GDT;


//Costanti riguardanti gli attributi di una entry della GDT




//------BYTE ACCESS(byte 5)------

//Bit 7: Present
#define D_PRESENT	0x80	//!< Il segmento e' "Presente"

//Bit 6-5: DPL(Descriptor Privilege Level)
#define DPL_0		0x00		//Descriptor Privilege Livel 0 (massimo privilegio).
#define DPL_1		0x20		//Descriptor Privilege Level 1.
#define DPL_2		0x40		//Descriptor Privilege Level 2.
#define DPL_3		0x60		//Descriptor Privilege Level 3 (minimo privilegio).

//Bit 4: S(system segment). Settato a 0 se e' un segmento di sistema, 1 se e' un code/data segment
#define S			0x10

//Bit 3: Executable
#define EXECUTABLE 0x8

//Bit 2: Expansion direction/conforming
#define EXPANSION 0x4

//Bit 1: Writable / Readable
#define WRITE_READ 0x2

//Bit 0: Accessed
//il bit Accessed viene settato ad 1 automaticamente quando si accede ad un' area di memoria utilizzando quel particolare descrittore

//------------------------------



//----Nybble Flags----
#define GRANULARITY 0x8
#define DEFAULT_SIZE 0X4
//-----------------------


#define STANDARD_CODE_SEGMENT (D_PRESENT | S | EXECUTABLE | WRITE_READ )
#define STANDARD_DATA_SEGMENT (D_PRESENT | S  | WRITE_READ )



#define KERNEL_CODE_SEGMENT (STANDARD_CODE_SEGMENT | DPL_0)
#define KERNEL_DATA_SEGMENT  (STANDARD_DATA_SEGMENT | DPL_0)

#define USER_CODE_SEGMENT (STANDARD_CODE_SEGMENT | DPL_3)
#define USER_DATA_SEGMENT  (STANDARD_DATA_SEGMENT | DPL_3)


#define TSS_SEGMENT ( D_PRESENT | DPL_0 | EXECUTABLE | 1)



#define DEFAULT_NYBBLE_FLAGS (GRANULARITY | DEFAULT_SIZE)
#define TSS_NYBBLE_FLAGS 0

//! This is the selector for the kernel code segment.
#define KERNEL_CODE		0x08
//! This is the selector for the kernel data segment.
#define KERNEL_DATA		0x10

//! This is the selector for the user code segment.
#define USER_CODE		0x18
//! This is the selector for the user data segment.
#define USER_DATA		0x20



#define SUPERVISOR_PRIVILEGE 0
#define USER_PRIVILEGE 3


#define KERNEL_CODE_SELECTOR	KERNEL_CODE
#define KERNEL_DATA_SELECTOR	KERNEL_DATA

#define USER_CODE_SELECTOR	USER_CODE | USER_PRIVILEGE	 //USER_PRIVILEGE sta praticamente ad indicare l' RPL
#define USER_DATA_SELECTOR 	USER_DATA | USER_PRIVILEGE	 //USER_PRIVILEGE sta praticamente ad indicare l' RPL
//Ricordo che il formato di un selettore e' del tipo:
//16bit:
/*
bit 0-1: RPL (Request Privilege Level)
bit 2: T (0: GDT; 1: LDT)
bit 3-15: Index(indice nella GDT)
*/






void load_GDT(struct gdt_descriptor* base,unsigned num_desc);
//void load_GDT(struct gdt_descriptor* base,unsigned num_desc);
unsigned int setup_GDT_entry(unsigned int base, unsigned int limit, unsigned char access, unsigned char flags_nybble);
void remove_GDT_entry(unsigned int sel);
void initGDT(void);


#endif

