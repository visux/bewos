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

/******************************************INFO*******************************************
* Coders: Black
* Descrizione: Definizioni di funzioni per la gestione della paginazione
********************************************************************************************/



#include<paging.h>
#include<task.h>
#include<system.h> //Per le macro save_flags e restore_flags

struct stack_struct stack_info;

/*Ricordare: 

Attraverso la paginazione e' possibile gestire la memoria considerando la memoria fisica come blocchi fissi generalmente di 4kb chiamate PAGINE. Quando si avvia il kernel la paginazione NON e' settata. Quindi tutto il kernel usa indirizzi fisici(indirizzi reali). Se poi viene abilitata la paginazione tutti gli indirizzi sono virtuali. Quindi quello che prima era un indirizzo reale(fisico) dopo che la paginazione è stata attivata diventa virtuale cioè viene considerato come pde/pte/off :

pde = PageDir Entry
pte = PageTable Entry
off = offset

La PagDir(pd) è una tabella(un array) in cui ogni elemento(entry) punta all' indirizzo fisico di una pagetable
La PageTable(pt) è una tabella(un array) in cui ogni elemento(entry) punta all' indirizzo fisico di un blocco di memoria(pagina)
l' Offset(off) è come dice il nome l' offset che localizza la precisa locazione di memoria all' interno del blocco di memoria puntato dalla pt

Attenzione: un entry(Sia della PT che della PD) è di 32bit

Dunque quando si ATTIVA la paginazione poichè  L' INDIRIZZO REALE VIENE CONSIDERATO COME UN INDIRIZZO VIRTUALE CIOE' DEL FORMATO PDE/PTE/OFF SI DEVE FARE IN MODO CHE ATTRAVERSO LE TABELLE PD E PT SI INDICIZZA UN INDIRIZZO REALE(CHE IN ALCUNI CASI PUO' COINCIDERE PROPRIO CON L'INDIRIZZO VIRTUALE).

Un indirizzo virtuale cioè del tipo PDE/PTE/off e' di 32bit scomposto in questo modo:

Primi 10bit a sinistra: individuano un entry nella PageDir
I 10bit intermedi: individuano un entry nella PageTable
Gli ultimi 12bit: forniscono un offset all' interno del blocco puntato dalla page table


ESEMPIO:

INDIRIZZO VIRTUALE: 0X12345678
in binario: | 0001001000 | 1101000101 | 011001111000 |
			pde   	 pte	       		off	       
		  
RICORDANDO CHE L' ENTRY DELLA PD E PT E' DI 10 BIT E L' OFFSET DI 12 BIT SI HA:

entry della pagedir = 0001001000
entry della pagetable = 1101000101
offset = 011001111000

Il processore utilizzerà questi valori nelle tabelle PageDir e PageTable per potersi riferire ad un indirizzo fisico.

In altri termini, tramite i primi 10bit viene ricavata un'entry nella pagedir che servira' a trovare il base phys. address di una page table. Tramite i 10bit intermedi verra' poi selezionata un'entry di tale page table, da cui sara' ricavato il base phys address della pagina in questione. Tramite il campo "offset" (relativo al base phys address della pagina) verra' poi ricavato l' indirizzo fisico di una locazione precisa di memoria.


Ad ogni processo è generalmente associato una ed una sola Pagedir le cui entry puntano ad una o più PageTable.
Siccome sia la Pagedir che la PageTable vengono memorizzati in un blocco di memoria che ha le stesse dimensioni di una pagina e cioè in questo caso di 4kb, il massimo numero di entry che possono avere e' di 1024. Infatti ricordando che un entry e' di 32bit cioe' 4byte e che 4Kb sono 4096byte basta fare 4096/4 = 1024. Dunque PageDir e PageTable possono insieme indirizzare 4Gb di memoria; infatti 1024*1024*4096byte = 4294967296byte ovvero 4GB. 

Un entry della PD o della PT non contiene soltando un indirizzo ma anche dei flags come "present bit", "dirty bit", "accessed bit", bit di privilegio e così via. In particolare l' indirizzo(che nel caso di un entry della PD e' l' indirizzo di una PT, mentre se si tratta di una PT e' l'indirizzo di un blocco di memoria fisica) viene memorizzato nei 20 bit piu significativi(i 20 bit a sinistra) dell' entry mentre gli altri 12 bit vengono utilizzati per i flags.


Importante: L' indirizzo base della PageDir e' contenuto nel registro cr3 noto anche come PDBR(Page Directory Base Register)

Recapitolando dunque un indirizzo virtuale deve essere "tradotto" in un indirizzo fisico. La traduzione viene effettuata da un dispositivi hardware chiamato MMU che legge dal PDBR l' indirizzo base della PD, i primi 10bit dell' indirizzo virtuale individuano un entry all' interno della PD, l' entry della PD punta ad una PT, i 10bit intermedi dell' indirizzo virtuale individuano un entry all' interno della PT, tale entry punta ad un blocco di memoria fisica(cioè ad una pagina), gli ultimi 12bit dell' indirizzo virtuale individuano la precisa locazione fisica di memoria all' interno di questo blocco. Viene cosi tradotto un indirizzo virtuale 

Ciò che noi dobbiamo fare e' inizializzare opportunamente PageDir e Pagatable. 


/* Durante l' esecuzione del sistema operativo con paginazione attivata ( generalmente per mezzo a delle chiamate alla funzione malloc(num); ) e' spesso necessario modificare PageDir e PageTable in modo da mettere(nel gergo mappare) degli indirizzo fisici(o piu in generale dei nuovi valori) alle entry di queste tabelle.

 
La domanda da porsi e': Come accedere direttamente alle entry della PageDir e alle entry della PageTable (all'interno delle quali ci sono indirizzi fisici)quando c' e' bisogno di modificare o leggere il valore di una entry ?

La domanda e' lecita, infatti, ricordo che il processore con paginazione attivata considera tutti gli indirizzi come virtuali e quindi li considera come pde/pte/off. Dunque se vogliamo modificare l' entry numero 5 della PageTable NON basta semplicemente fare(ad esempio): PageTable[5] = nuovo_valore;  
Infatti l' indirizzo dove e' presente l' array PageTable verrebbe considerato come un indirizzo virtuale e cioe' come pde/pte/off. Dunque verrebbe tradotto come un indirizzo fisico secondo le entry pde e pte. L' indirizzo fisico tradotto nella maggior parte dei casi non e' mai uguale a quello virtuale(tranne in alcuni casi in cui e' necessario che l' indirizzo virtuale sia tradotto come lo stesso indirizzo fisico) e dunque non sara' uguale a quello dell' array PageTable. In Questi casi generalmente avviene una eccezzione denominata Page-Fault(eccezione numero 14) poiche' e' probabile che a quell' indirizzo virtuale non e' associato nessun indirizzo fisico..


Questo "problema" potrebbe essere ovviato disabilitando la paginazione, modificare l' entry che vogliamo della PageDir o della PageTable e ri-abilitare la paginazione. Per motivi di efficienza questo approccio e' da scartare(tranne in alcuni casi che e' quasi necessario).

Esistono vari approcci, ma quello piu efficiente(almeno secondo alcuni ed anche secondo me) e' inserire(mappare) la PageDir all' interno di se stessa all' ultima sua entry(quindi all' entry 1023esima).
Praticamente: PageDir[1023] = PageDir;

ed e' in questi casi che la paginazione NON deve essere attivata altrimenti ci sarebbe lo stesso problema.


Vediamo come funzione questo approccio:

Sappiamo che un indirizzo virtuale e' di 32 bit diviso in 10 bit di PDE, 10 bit di PTE, 12 bit di OFF. Quando il processore si riferisce ad un indirizzo virtuale questo deve essere tradotto. Dunque ricordando che ogni entry e' di 4byte raggiunge l'indirizzo: CR3+PDE*4
e legge l' entry; tale entry individua l' indirizzo fisico di una PageTable(PTAddr). Poi raggiunge l' indirizzo PTAddr+PTE*4 e legge l' entry; tale entry individua l' indirizzo fisico(PageAddr) di un blocco di memoria(di una pagina fisica); infine termina la "traduzione" raggiungendo l' indirizzo PageAddr+OFF che individua la locazione di memoria da leggere o su cui scrivere.

Ora nel caso in cui vogliamo modificare/leggere una entry della PD o di una PT bisogna "costruire" appositamente un indirizzo in modo tale che i 10 bit di PDE individueranno l' entry 1023; i bit di PTE individueranno la PT da modificare; l' OFF individuera' l' entry da editare nella PT :

PDE = 1023
PTE = n_PT da editare
OFF = entry da editare nella pt


se si vuole modificare/leggere un entry della PD si costruisce un indirizzo del tipo:

PDE = 1023
PTE = 1023
OFF = entry da editare nella pd


Dunque nel caso in cui l' ultima entry della PD sia un puntatore ad essa il processore raggiunge l' indirizzo CR3+PDE*4;
legge l'entry; nell'entry c' e' l' indirizzo fisico della pd stessa(PDAddr); quindi fa PDAddr+PTE*4; legge l' entry, ma attenzione ci troviamo ancora nella PageDir quindi l' entry che verra' letta dara'  l' indirizzo fisico della PageTable che si intende modificare(PTAddr); infine fara' PTAddr+OFF*4. Ci troviamo cosi' all' interno della PageTable e sara' quindi possibile modificare/leggere il valore dell' entry che si intendeva modificare/leggere.

Lo stesso ragionamente vale se si intende modificare/leggere un entry della PageDir


Esempio:

Mettiamo il caso che vogliamo modificare l'entry numero 5 della PT numero 7

costruiamo appositamente un indirizzo del tipo: addr=(1023<<22)+(5<<12)+(7*4)


questo indirizzo e' virtuale ma il processore quando lo traduce in un indirizzo fisico esso puntera' all'indirizzo FISICO della PT all'entry numero 7 quindi la si puo' modificare/leggere liberamente. */






/* Le funzioni che seguono si occuperanno di gestire la paginazione */


/* La funzione che segue permette di abilitare la paginazione */
 void enable_paging(void)
{

	unsigned int control;
	
	    /* Il bit 31 di CR0 (flag PG) e' responsabile dell'utilizzo
	      della paginazione e viene settato solitamente dal SO durante
	      l'inizializzazione. Per poter ABILITARE la paginazione deve essere posto a 1 */

	 /* Leggo prima il valore del registro cr0 e assegno il valore letto alla variabile control */
	asm volatile ("mov %%cr0, %0":"=r" (control));
	
	/* Setto il bit 31 a 1 e assegno il valore ottenuto alla variabile control */
	control |= 0x80000000;
	
	/* Scrivo il valore della variabile control nel registro cr0 abilitando così la paginazione */
	asm volatile ("mov %0, %%cr0"::"r" (control));
	
	/* ricarico i registri di segmento */
	asm volatile
	("movw $0x10,%%ax     \n"
	"movw %%ax,%%ds      \n"
	"movw %%ax,%%es      \n"
	"movw %%ax,%%fs      \n"
	"movw %%ax,%%gs      \n"
	"movw %%ax,%%ss      \n"
	"ljmp $0x08,$next    \n"
	"nop\n" "nop\n"
	"next:               \n":::"%eax");
	return;
		
}


/* La funzione che segue permette di disabilitare la paginazione */
void disable_paging(void)
{
	unsigned int control;
	
	    /* Il bit 31 di CR0 (flag PG) e' responsabile dell'utilizzo
	      della paginazione e viene settato solitamente dal SO durante
	      l'inizializzazione. Per poter DISABILITARE la paginazione deve essere posto a 0 */

	 /* Leggo prima il valore del registro cr0 e assegno il valore letto alla variabile control */
	asm volatile ("mov %%cr0, %0":"=r" (control));
	
	/* Setto il bit 31 a 0 e assegno il valore ottenuto alla variabile control */
	control ^= 0x80000000;
	
	/* Scrivo il valore della variabile control nel registro cr0 disabilitando così la paginazione */
	asm volatile ("mov %0, %%cr0"::"r" (control));
	
	/* ricarico i registri di segmento */
	asm volatile
	("movw $0x10,%%ax     \n"
	"movw %%ax,%%ds      \n"
	"movw %%ax,%%es      \n"
	"movw %%ax,%%fs      \n"
	"movw %%ax,%%gs      \n"
	"movw %%ax,%%ss      \n"
	"ljmp $0x08,$next2    \n"
	"nop\n" "nop\n"
	"next2:               \n":::"%eax");
	return;
	
}	



/* La funzione che segue inizializza la paginazione prima di abilitare la paginazione stessa */
void init_paging(void)
{
long flags;

		save_flags(flags); //salvo gli Eflags nella variabile flags attraverso la macro save_flags
		asm("cli"); //Disabilito gli interrupt

/* 
Mappo 4Mb per il kernel e i dati di paginazione. Dunque creo una pagedir ed una pagetable(1024 * 4Kb = 4Mb). 

Per i primi 4Mb faccio in modo che ad un indirizzo virtuale corrisponda lo stesso indirizzo fisico.

Praticamente se ho un indirizzo fisico 8272 dopo che è stata attivata la paginazione questo indirizzo viene considerato come indirizzo
virtuale(pd/pt/off). Quel che si deve fare è inizializzare opportunamente pagedir e pagetable in modo che l' indirizzo virtuale 8272 venga
tradotto come indirizzo fisico 8272 (la corrispondenza virtuale/fisico vale solo per il kernel, ma anche per altre cose).

esempio:

indirizzo fisico: 8272

dopo che è stata attivata la paginazione si ha:

indirizzo virtuale: 8272  cioè:

in binario 8272 è: 00000000000000000010000001010000

quindi ricordando che i primi 10 bit individuano l' entry nella pagedir, i secondi 10 bit l' entry nella pagetable e gli ultimi 12 bit l' offset
si ha:

pdentry: 0000000000 (in decimale 0)
ptentry:  0000000010 (in decimale 2)
offset:    000001010000 (in decimale 80)


Bisogna inizializzare opportunamente la tabella "page dir" e la tabella "page table" in modo che all' indice 0 della page dir corrisponda
una page table che all' indice 2 punterà ad un indirizzo al quale aggiungendo l' offeset 80 darà l' indirizzo fisico 8272.

Per fare questo si utilizza il codice sottostante. 
*/

//Metto la page_dir e la page_table in indirizzi che conosco a priori
unsigned long* page_dir = (unsigned long*)0x1000;
unsigned long* page_table = (unsigned long*)0x2000;

unsigned long address = 0, pd_addr;
unsigned int i;

//inizializzo la page table
for(i=0; i<1024; i++)
{
	page_table[i] = address | KERNEL_PAGES_FLAG;
	address = address + 4096;
}

//Assegno alla page_dir di indice 0 l' indirizzo della page table
page_dir[0] = (unsigned long)page_table;
page_dir[0] = page_dir[0] | KERNEL_PAGES_FLAG;



/* Setto tutte le altre entry della page dir come non presenti. Per fare questo basta impostare solo il flag di lettura e scrittura */
for(i=1; i<1024; i++)
  page_dir[i] = 0 | READ_WRITE;
  
 /* All' ultimo elemento della PageDir ffaccio puntare la PageDir stessa */ 
  page_dir[1023] = (unsigned long)page_dir;
  page_dir[1023] = page_dir[1023] | KERNEL_PAGES_FLAG;
  

  
/* 
Ritornando all' esempio precedente la page dir di indice 0 punta alla page table che all' indice 2 varrà 8192 cioè punterà all' indirizzo fisico 8192, a questo punto il processore aggiunge l' offset 80 e si ha l' indirizzo fisico:

8192 + 80 = 8272

Come si puo vedere attraverso il pezzo di codice precedente è possibile fare in modo che l' indirizzo virtuale xyz corrisponda proprio all' indirizzo fisico xyz. La corrispondenza conviene farlo solo con il kernel. 
*/

  
  /* Affinche' il processore sappia l' indirizzo della page dir bisogna inserire questo indirizzo nel registro cr3 chiamato anche PDBR
     (Page Directory Base Register) */
 pd_addr = (unsigned long int) page_dir & 0xFFFFF000; //Il valore da inserire nel registro cr3
  asm volatile("movl %0, %%cr3\n" : : "r"(pd_addr)); //Inserisco il valore nel registro cr3


/* Ora che ho settato il tutto posso abilitare la paginazione */
enable_paging(); 

		restore_flags(flags);  //Ripristino gli Eflags

return;

}


/* La funzione che segue inserisce un indirizzo fisico(dato in ingresso) nelle coordinate PDE / PTE espresse dall' indirizzo
virtuale(quindi del tipo pde/pte/off) dato in ingresso */
void mappage(unsigned int virtual_address, void* real_address, unsigned int flags)
{
int pd_entry, pt_entry;


pd_entry=(virtual_address>>22)&0x3FF; //Mi prendo il numero di PD Entry dall' indirizzo virtuale
pt_entry=(virtual_address>>12)&0x3FF; //Mi prendo il numero di PT Entry dall' indirizzo virtuale

edit_pte(pd_entry, pt_entry, real_address, flags);  /* Chiamo la funzione che modifica la "pt_entry" nella PageTable. Il nuovo valore
che sara' inserito sara' l' indirizzo reale(fisico) con i flag dati in ingresso */

}


//La funzione che segue crea e restitusce un indirizzo virtuale apposito dell' entry della PageTable che si vuole leggere o scrivere
//Riceve in ingresso il numero della pt da editare e l' entry della pt da editare
unsigned long return_virt_addr_pte(unsigned int n_pt, unsigned int pt_entry)
{
return (unsigned long)((1023<<22) + (n_pt<<12) + (pt_entry*4));
}


//La funzione che segue crea e restitusce un indirizzo virtuale apposito dell' entry della PageDir che si vuole leggere o scrivere
//Riceve in ingresso il numero dell' entry dell page dir da editare
unsigned long return_virt_addr_pde(unsigned int pd_entry)
{
return (unsigned long)((1023<<22) + (1023<<12) + (pd_entry*4));
}



/*
La funzione che segue permette di editare un entry della PageDir

num_pde: numero dell' entry della pd da editare
address: nuovo indirizzo da mettere nell' entry ' num_pde'
flags: flags
*/
void edit_pde(unsigned int num_pde, void* address, unsigned int flags)
{
unsigned long* pd_entry;
pd_entry = (unsigned long*)return_virt_addr_pde(num_pde); //Indirizzo virtuale dell' entry della pagedir da editare
*pd_entry = ((unsigned long)address | flags);
}


/*
La funzione che segue permette di editare un entry di una PageTable

num_pde: numero dell' entry della pd da editare
num_pte: numero dell' entry della pt da editare
address: nuovo indirizzo da mettere nell' entry ' num_pde'
flags: flags
*/
void edit_pte(unsigned int num_pde, unsigned int num_pte, void* address, unsigned int flags)
{
unsigned long* pt_entry;
pt_entry = (unsigned long*)return_virt_addr_pte(num_pde, num_pte); //Indirizzo virtuale dell' entry della pagetable da editare
*pt_entry = ((unsigned long)address | flags);
}


/*Questa funzione legge il valore di un entry della PageDir. Prende in ingresso il numero dell' entry che si vuole leggere*/
unsigned long read_pde(unsigned int num_pde)
{
unsigned long* pd_entry;
pd_entry = (unsigned long*)return_virt_addr_pde(num_pde); //Indirizzo virtuale dell' entry della PageDir da leggere
return *pd_entry;
}


/*Questa funzione legge il valore di un entry della PageTable. Prende in ingresso il numero dell' entry della PageDir e dell' entry
della PageTable che si vuole leggere*/
unsigned long read_pte(unsigned int num_pde, unsigned int num_pte)
{
unsigned long* pt_entry;
pt_entry = (unsigned long*)return_virt_addr_pte(num_pde, num_pte); //Indirizzo virtuale dell' entry della pagetable da editare
return *pt_entry;
}


/* La funzione che segue restitusce la prima PDE trovata libera cioe' non occupata(NOT PRESENT) */
unsigned int find_first_pde_free(void)
{
unsigned int pd_entry;
unsigned long pd_entry_value;

for(pd_entry=0; pd_entry < 1023; pd_entry++)
 {
 
 pd_entry_value = read_pde(pd_entry); /* Leggo di volta in volta tutti i valori delle entry della PageDir fin quando non ne trovo uno
 NOT PRESENT cioe' libero */

   if(is_present(pd_entry_value) == 0 ) //Se trovo un entry della PageDir libera
    break; //esci dal ciclo for
 }



if(pd_entry == 1023) //se pd_entry == 1023 signica che non ho trovato entry libere
 return -1; //per questo ritorno -1

else
 return pd_entry;  //Altrimenti ritorno l' indice dell' entry della PageDir trovata libera
}



/* La funzione che segue trova la prima PTE libera(NOT PRESENT) dove poter inserire(mappare) un indirizzo fisico. Restituisce un
indirizzo virtuale(pd/pt/off) */
void* find_first_pte_free(void)
{
int pd_entry, pt_entry;
unsigned long pd_entry_value, pt_entry_value;
void * virtual_address;

for(pd_entry=0; pd_entry < 1023; pd_entry++)
 {
  pd_entry_value = read_pde(pd_entry);

   if(is_present(pd_entry_value) == 1 ) //se l' entry della PageDir e' presente
     {

	 for(pt_entry=0; pt_entry < 1024; pt_entry++) //scansiono la PageTable alla ricerca di un entry libera(NOT PRESENT)
	 {

		 pt_entry_value = read_pte(pd_entry, pt_entry);
		 
		  if(is_present(pt_entry_value) == 0 ) //Se trovo un entry della PageTable libera
		  {
		 	 virtual_address = make_virtual_address(pd_entry, pt_entry, 0);
		  
		  	//Controllo se l' indirizzo virtuale e' riservato allo stack. Se e' cosi' continuo con il ciclo
		 	 if(virtual_address >= stack_info.start_stack_virtual_addr && virtual_address <= stack_info.end_stack_virtual_addr)
			   continue;
			  else //altrimenti ... 
     			   break; //esco dal ciclo del secondo for   
		}
	 }

	  if(is_present(pt_entry_value) == 0 ) //Se ho trovato un entry della PageTable libera
	    break; //esco dal ciclo del primo for
     }

 }


/*se pd_entry == 1023 signica che non ho trovato entry libere(1023 non 1024 perche' l' ultima entry e' riservato alla pd stessa)*/
if(pd_entry == 1023) 
 return (void*)-1; //per questo ritorno -1

else
 return virtual_address;  /* Altrimenti ritorno l' indirizzo virtuale che non e' altro che le
  coordinate(pde/pte/off) dell' entry della PageTable trovata libera */

}

/* La funzione che segue permette di liberare le entry della PageDir e della PageTable quando e' necessario */
void free_pages(void* virtual_address)
{
int pd_entry, pt_entry, pte_control, control_pd;
unsigned long control_virt_addr;
unsigned long pt_entry_value, pd_entry_value;



pd_entry=((unsigned long)virtual_address>>22)&0x3FF;  //Mi prendo il numero di PD Entry dall' indirizzo virtuale
pt_entry=((unsigned long)virtual_address>>12)&0x3FF;  //Mi prendo il numero di PT Entry dall' indirizzo virtuale



	 for(pte_control=0; pte_control < 1024; pte_control++) //scansiono la PageTable
	 {

		 pt_entry_value = read_pte(pd_entry, pte_control); //Leggo di volta in volta i vari valori delle entry della PageTable

		/* Se trovo un entry della pagetable PRESENTE che non sia quella che voglio cancellare... */
		  if(is_present(pt_entry_value) == 1 && pte_control != pt_entry) 
     		   break; //esco dal ciclo
	 }


if(is_present(pt_entry_value) == 1 && pte_control != pt_entry) //Se ho trovato un entry della PageTable PRESENTE
{

pt_entry_value = read_pte(pd_entry, pt_entry); //Leggo il valore dell'entry "pt_entry"
pt_entry_value = pt_entry_value & 0xFFFFF000; //mi prendo solo l' indirizzo fisico a cui punta l' entry "pt_entry"
phisic_free((void*)pt_entry_value); //dealloco fisicamente l' indirizzo fisico a cui punta l' entry "pt_entry"

edit_pte(pd_entry, pt_entry, 0, READ_WRITE); /* setto solamente l' entry della PageTable come NOT PRESENT rendendola libera e
quindi riutilizzabile */
}

else /* altrimenti significa che posso deallocare fisicamente la PageTable perche' oltre all' entry che voglio cancellare non ce ne sono
 altre */
{

pt_entry_value = read_pte(pd_entry, pt_entry); //Leggo il valore dell'entry "pt_entry"
pt_entry_value = pt_entry_value & 0xFFFFF000; //mi prendo solo l' indirizzo fisico a cui punta l' entry "pt_entry"
phisic_free((void*)pt_entry_value); //dealloco fisicamente l' indirizzo fisico a cui punta l' entry "pt_entry"


pd_entry_value = read_pde(pd_entry); 

pd_entry_value = pd_entry_value & 0xFFFFF000 ; //mi prendo solo l' indirizzo fisico della PageTable senza flag
phisic_free((void*)pd_entry_value); //Dealloco fisicamente la PageTable
edit_pde(pd_entry, 0, READ_WRITE); /* poichè ho cancellato la PageTable setto come NOT PRESENT la entry della PageDir che
riferiva alla PageTable cancellata */
}

return;

}



/* La funzione che segue stampa il contenuto della PageDir. Prende in ingresso l' entry di cui si vuole sapere il valore */
// pd_entry == -1: stampa tutta la page dir 
void print_pagedir(int pd_entry)
{

unsigned long pd_entry_value;

	if(pd_entry == -1)
	{
		/* stampa tutta la PageDir */
		for(pd_entry=0;pd_entry<1024;pd_entry++)
		{
		      	pd_entry_value = read_pde(pd_entry); //Leggo di volta in volta il valore delle entry della PageDir
			//if(is_present(*pd_addr) == 1) //stampa solo le entry occupate (PRESENT)
			printk("PAGEDIR[%d]: table_address = %d\n",pd_entry, pd_entry_value); /* Stampo di volta in volta il valore delle
			 entry della PageDir */ 

		}
		
		
	return;
	
	}
		//Se pd_entry non e' -1 stampo solo il valore dell' entry "pd_entry"
		   pd_entry_value = read_pde(pd_entry); //Leggo il valore dell' entry "pd_entry" della PageDir
		   printk("PAGEDIR[%d]: table_address = %d\n",pd_entry, pd_entry_value); //stampo il valore
	 
	
	return;
}


 //La funzione che segue controlla se il flag PRESENT del valore dell' entry(della PD o della PT) dato in ingresso e' settato o meno
int is_present(unsigned long entry)
{

if((entry & PRESENT) == 0) // Se NON e' settato 
 return 0; //restituisco 0
else 
 return 1; //altrimenti restituisco 1
 
}

//La funzione che segue crea e restitusce un indirizzo virtuale(pde/pte/off) in base alla PDE, alla PTE e all' offset dato ingresso
void* make_virtual_address(unsigned int pde, unsigned int pte, unsigned int offset)
{
	return (void *)((pde << 22) + (pte << 12) + offset);
}
 

