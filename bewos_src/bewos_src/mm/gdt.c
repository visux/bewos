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

/******************************************INFO*****************************************************************
* Coders: Black
* Descrizione: Definizioni di funzioni per l' inizializzazione e gestione della *G*lobal *D*escriptor *T*able
******************************************************************************************************************/




#include<gdt.h>
#include<string.h> //La includo per la define NULL
#include<system.h> //Per le macro save_flags e restore_flags
#include<io.h>

/* GDT e' acronimo di Global Descriptor Table ed e' a livello pratico una array di strutture. La GDT permette di descrivere un blocco di memoria(i segmenti di memoria) attraverso i suoi elementi che prendono il nome di "descrittori". Un descrittore in linguaggio 'C' non e' altro che una struttura che ha particolari campi. Tali campi "descrivono" un segmento di memoria. E' possibile vedere il formato di tale struttura nel file gdt.h */

struct gdt_descriptor *GDT;		//Dichiaro la GDT

struct x86_TSS *TSS;		/* Dichiaro la TSS che e' un particolare descrittore della GDT. Almeno UNA TSS e' necesaria affinche' possa avvenire lo switch tra Task che operano in user mode(ring 3) e kernel mode(ring 0). TSS sta per Task State Segment ed e' un particolare blocco di memoria(una normale struttura C) che tiene conto dello stato di un task. Generalmente vi e' una TSS per ogni task creato, ma siccome bewos utilizza un multitasking software(al contrario di un multitasking hardware) non ha bisogno di avere una TSS per ogni task creato. Il multitasking software, infatti, salva lo stato di un task nella struttura dati del task stesso e lo ripristina quando e' necessario(si veda il file task.c e task.h). MA se si vuole gestire lo switch tra user mode e kernel mode con un multitasking software e' necessario tuttavia avere una sola TSS. La struttura di una TSS e' presente nel file gdt.h ed essa comprende vari campi. Quelli che utilizzeremo noi sono soltanto ESP0, SS0, IO_bitmap_offset e bitmap_io_permission e solo questi sono necessari per lo switch di privilegio. Ad ogni interrupt il processore imposta lo stack in base al valore di esp0 ed ss0 della TSS correntemente caricata. Quindi le successive operazioni di push e pop avverrano su questo stack. In pratica quando si verifica lo switch ring3->ring0 il processore automaticamente carica nel registro esp il valore di TSS->ESP0 e nel registo ss il valore di TSS->SS0.  
In BeWos ogni task ha un kernel stack e, se e' stato creato con privilegio USER, avra' anche un user stack. TSS->ESP0 sara' sempre uguale al kernel stack del task corrente! Quando si switcha(cambia) task, lo scheduler, infatti, imposta TSS->ESP0 con il valore del kernel stack del task che andra' in esecuzione.
 
Il kernel stack e' (detto semplicisticamente) lo stack dove ad ogni interrupt vengono messi(pushati) i registri per salvare lo stato del task corrente. L' user stack viene utilizzato per tutte le altre cose. Se avviene un interrupt mentre ci troviamo in ring3 vi e' ovviamente(scusate l' ovvieta') uno switch di livello, infatti, gli interrupt "lavorano" a ring0. Dunque, siccome si passa da ring3 a ring0, nel registro esp verra' caricato automaticamente il valore di TSS.ESP0; come gia'  detto, all' interno di tale stack verranno salvati i valori dei registri cs, eflags, ds, ss ecc...(in pratica verra' salvato lo stato del task corrente). Da notare che nel registro esp viene messo il valore di TSS.ESP0 solo se si passa da ring3 a ring0 e NON il contrario. Lo stesso valer per ring0->ring0. In tal proposito si vedano i files: idt.c, task.c, task.h e scheduler.c. */




/* La funzione "load_GDT" carica la GDT in un particolare registro chiamato 'gdtr' attraverso una particolare istruzione, ovvero 'lgdt'. */
void load_GDT(struct gdt_descriptor* base,unsigned int num_desc) {


/* il registro gdtr e' di 48bit(6 byte). Nei primi due byte viene specificata la dimensione della GDT(2^16 = 8192. Questo spiega per quale motivo il massimo numero di descrittori che la GDT puo' contenere e' 8192). Negli altri 4byte viene specificato l' indirizzo base della GDT */
	unsigned GDT_reg[2];

	GDT_reg[0]=(num_desc*8-1) << 16;
	GDT_reg[1]=(unsigned int)base;

	           asm("": : :"memory");
	__asm__ __volatile__ ("lGDT (%0)": :"g" ((char *)GDT_reg+2));


	//Carico la TSS nel registro TR (Task Register)
	asm("movw $0x28,%ax"); /*0x28 = 101000 : i bit 101 = 5 indicano il selettore, gli altri bit indicano altre cose(GDT\LDT, RPL). l'
	 indice e' '5' perche'  nella GDT abbiamo messo il descrittore della TSS proprio all' indice '5' */
	asm ("LTR %ax");


	
}





//La funzione sottostante "installa" un descrittore nella GDT con gli attributi che la funzione stessa riceve in ingresso
unsigned int setup_GDT_entry(unsigned int base, unsigned int limit, unsigned char access, unsigned char flags_nybble)
{

	register unsigned short i = 1; //L' entry 0 e' riservata. Per questo inizializziamo l' indice a 1


	for( ; i < GDT_MAX_ENTRY; i++ )
	{
		if(GDT[i].access == 0 && GDT[i].flags_nybble == 0) //controllo se l'entry e' vuoto
		{
			GDT[i].limit0_15 = limit & 0xFFFF;
			GDT[i].limit16_19 = (limit >> 16) & 0x0F;
			GDT[i].base0_15 = base & 0xFFFF;
			GDT[i].base16_23 = (base >> 16) & 0xFF;
			GDT[i].base24_31 = (base >> 24) & 0xFF;
			GDT[i].access = access;
			GDT[i].flags_nybble = flags_nybble;


			// Ritorna il selettore
			return(i * GDT_ENTRY_DIM);
		}
	}
	//Se non trova posti liberi nella GDT ritorno NULL
	return((unsigned int)NULL );
}

//la funzione che segue permette di rimuovere una entry della GDT
void remove_GDT_entry(unsigned int sel)
{

	GDT[sel/GDT_ENTRY_DIM].limit0_15		=	0;
	GDT[sel/GDT_ENTRY_DIM].limit16_19		=	0;
	GDT[sel/GDT_ENTRY_DIM].base0_15		=	0;
	GDT[sel/GDT_ENTRY_DIM].base16_23		=	0;
	GDT[sel/GDT_ENTRY_DIM].base24_31		=	0;
	GDT[sel/GDT_ENTRY_DIM].access		=	0;
	GDT[sel/GDT_ENTRY_DIM].flags_nybble	=	0;
	
}


//La funzione sottostante inizializza la GDT
void initGDT(void)
{
int i;
long flags;

		save_flags(flags); //salvo gli Eflags nella variabile flags attraverso la macro save_flags
		asm("cli"); //Disabilito gli interrupt

/* Con l' istruzione seguente non facciamo altro che assegnare "dello spazio" alla GDT che abbiamo dichiarato precedentemente. USE_N_ENTRY E' una costante che per adesso in bewos vale 6 ed indica il numero di segmenti di memoria che vengono utilizzati da BeWos. GDT_ENTRY_DIM vale 8 ed indica la dimensione dei descrittori. dunque kmalloc riserva 6*8 = 48 byte per la GDT */
GDT = (struct gdt_descriptor*)kmalloc(USE_N_ENTRY*GDT_ENTRY_DIM);

//Inizializzo la GDT
for(i = 0; i<USE_N_ENTRY; i++)
 {
 	GDT[i].limit0_15		=	0;
	GDT[i].limit16_19		=	0;
	GDT[i].base0_15		=	0;
	GDT[i].base16_23	=	0;
	GDT[i].base24_31	=	0;
	GDT[i].access		=	0;
	GDT[i].flags_nybble	=	0;
 }
 
 
//Riservo spazio per la TSS
 TSS = (struct x86_TSS*)kmalloc( sizeof(struct x86_TSS));
 
 
/* BeWos utilizza due segmenti per codice e dati con privilegio 0(Supervisor) e due segmenti codice e dati con privilegio 3(User)
I propri descrittori vengono inseriti nella GDT attraverso le funzioni seguenti */

//	Base: 0,	Limite: 4GB		Tipo Segmento: Codice		Privilegio: Supervisor
setup_GDT_entry(0, 0xFFFFF, KERNEL_CODE_SEGMENT, DEFAULT_NYBBLE_FLAGS);
//	Base: 0,	Limite: 4GB		Tipo Segmento: Dati		Privilegio: Supervisor
setup_GDT_entry(0, 0xFFFFF, KERNEL_DATA_SEGMENT, DEFAULT_NYBBLE_FLAGS);

 //	Base: 0,	Limite: 4GB		Tipo Segmento: Codice	Privilegio: User
setup_GDT_entry(0, 0xFFFFF, USER_CODE_SEGMENT, DEFAULT_NYBBLE_FLAGS);
//	Base: 0,	Limite: 4GB		Tipo Segmento: Dati	Privilegio: User
setup_GDT_entry(0, 0xFFFFF, USER_DATA_SEGMENT, DEFAULT_NYBBLE_FLAGS);

/*Notare che il limite dei descrittori e' 0xFFFFF, ma esso viene moltiplicato per 4096 se il bit di granularita' e' attivato come in questo caso( 0xFFFFF * 4096 = 4GB). Per maggiori info su tale bit si veda il file gdt.h :) */

/* Installo nella GDT il descrittore per la TSS. Notare che come base(primo argomento) passo l' indirizzo base della TSS e come limite(secondo argomento) passo la dimensione della TSS */
setup_GDT_entry((unsigned)TSS, sizeof(struct x86_TSS), TSS_SEGMENT, TSS_NYBBLE_FLAGS);


//Imposto SS0 della TSS con il selettore KERNEL_DATA_SELECTOR
TSS -> SS0 = KERNEL_DATA_SELECTOR;

/* Il campo sottostante contiene un offset(a partire dall' inizio della TSS) che si riferisce ad una bitmap(un array dichiarata nella stessa TSS all' ultimo campo) di cui ogni bit rappresenta una porta di Input\Output. La bitmap e' presente al 104esimo byte all' interno della TSS */
TSS -> IO_bitmap_offset = 104;

/* Inizializiamo l' array a cui fa riferimento "IO_bitmap_offset". Ogni bit all' interno della bitmap rappresenta una porta di Input\Output. il bit 5 rappresenta la porta 5, il bit 145 rappresenta la porta 145 e cosi' via... Se un bit e' posto a 0 significa che a quella specifica porta e' possibile accederci altrimenti non e' possibile. Le porte di I\O sono 8192. Ogni elemento della bitmap e' di 32bit.  Quindi gli elementi dell' array saranno 8192/32. Notare che inizializzo tutti i bit a 0; questo significa che faccio accedere a tutte le porte. */
for(i=0; i<(8192/32);i++)
TSS->bitmap_io_permission[i] = 0x00000000;


//Carichiamo la GDT
load_GDT(GDT, USE_N_ENTRY);



/* ora che abbiamo caricato la GDT nel registro 'gdtr' attraverso la funzione "load_GDT" non ci resta altro che settare opportunamente i registri segmento. In modalita' protetta i registri segmento(cs, ds, es, fs, gs,ss) vengono considerati come dei "selettori". Infatti essi vengono considerati come indici che individuano un descrittore all' interno della GDT.


Il formato di un selettore(16bit) e' il seguente

bit 0-1: RPL. Indica il livello di privilegio richiesto. Se e' maggiore(quindi se ha meno privilegio) del DPL del descrittore a cui si cerca di accedere si verifica un eccezione: #General Protection Fault

bit 2: TI. Specifica a quale tabella dei descrittori fare riferimento. 0 per la GDT, 1 per la LDT(che e' un altro "tipo" di tabella che BeWos non utilizza).

bit 3-15: Index. Indica la posizione all' interno della GDT del descrittore del segmento a cui si vuole accedere.


qui di seguito inizializzo opportunamente i registri segmento(i selettori in pratica) */

	asm volatile (
		"movw %0,%%ax    \n"
		"movw %%ax,%%ds  \n"
		"movw %%ax,%%es  \n"
		"movw %%ax,%%fs  \n"
		"movw %%ax,%%gs  \n"
		"movw %%ax,%%ss  \n"
		"ljmp %1,$next   \n" //faccio lo jump a CS
		"nop\n"
		"nop\n"
		"next:\n" :: "i" (KERNEL_DATA), "i" (KERNEL_CODE));
		
		
		
/* Queste righe di codice in asm inline mettono il valore di KERNEL_DATA nei registri ds, es, fs, gs, ss. Dopodiche' attraverso questa istruzione: "ljmp %1,$next" carica il valore di KERNEL_CODE nel registro cs.

KERNEL_CODE e KERNEL_DATA valgono rispettivamente 0x8 e 0x10. Questo perche' abbiamo messo i descrittori di kernel code e kernel data all' indice 1 e 2 della GDT. Ricordandoci il formato del selettore, 0x8 viene scomposto e considerato dal processore in questa maniera:

0x8 = 0000000000001000 (in binario)

bit 0-1(RPL) = 0(Supervisor)

bit 2(TI) = 0(GDT)

bit 3-15(Index) = 1



Analogamente viene considerato 0x10:


0x10 = 0000000000010000 (in binario)

bit 0-1(RPL) = 0(Supervisor)

bit 2(TI) = 0(GDT)

bit 3-15(Index) = 10(che in decimale sarebbe 2)



Noi carichiamo "proprio" tali valori(0x8 e 0x10) all' interno dei registri di segmento(selettori) perche' i descrittori kernel code e kernel data sono stati messi "proprio" all' indice 1 e 2 della GDT. All' indice 3 e 4 sono stati, invece, messi i descrittori dei segmenti codice e data User. Per descrittori "kernel code e kernel data" si intendono descrittori con DPL = 0. Per descrittori "user code e user data" si intendono descrittori con DPL = 3. 
Quando si intende fare lo switch(cambio) di privilegio(ad esempio da kernel mode a user mode) "basta" settare cs e ds con "opportuni valori" in modo da far riferimento a descrittori(codice e data) con DPL = 3 */		
		
		

		restore_flags(flags);  //Ripristino gli Eflags


	return;
}


