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
 
/******************************************INFO******************************************************
* Coders: Black
* Descrizione: Definizioni di funzioni per l' inizializzazione e gestione della memoria fisica
*******************************************************************************************************/


/* RICORDA: la gestione della memoria fisica e' diversa dalla paginazione vera e propria. Infatti per gestire la memoria fisica(RAM)
utilizzo soltanto la logica della paginazione  che mi permette di allocare e deallocare pagine in base alla quantita' di memoria calcolata
disponibile. Quindi tramite le funzioni che seguono posso tenere traccia in modo astratto della memoria fisica(pagine disponibili, occupate, libere ecc..). */

/* Premesse: */
//- Ogni pagina e' di 4kb cioe' di 4096 byte
//- Per calcolare quante pagine si hanno a disposizione basta fare il rapporto tra la memoria fisica totale e 4096
//- Utilizzeremo un vettore, la bitmap, che tiene traccia delle pagine. Il vettore conterra' elementi da 4 byte cioe' da 32 bit e considereremo
//  che ogni bit e' una pagina; quindi un elemento che e' di 4 byte equivale a 32 pagine.
//- Per sapere il numero di  elementi della bitmap che ci servono realmente basta fare il rapporto tra le pagine che si hanno a disposizione
//   e 32( ricordo che ogni singolo bit equivale ad una pagina e gli elementi del vettore sono di 32bit ciascuno)
//- Le pagine occupate verranno indicate con 0xF  e le pagine libere con 0x0. Quindi i bit della bitmap se sono posti a 0 indicano delle 
//  pagine libere, se sono posti a 1 indicano delle pagine occupate.

/* Le funzioni principali che verranno implementate riguarderanno il calcolo della memoria fisica disponibile,  l' inizializzazione della
bitmap, l' occupazione(allocazione) di num pagine libere all' interno della bitmap ritornando l' indirizzo fisico di partenza,  infine la
liberazione delle pagine occupate che diventeranno nuovamente libere */



/***** START IT *****/

#include<phisicmem.h>
#include<io.h>
#include<system.h> //Per le macro save_flags e restore_flags
#define NULL 0
 
 unsigned int bitmap[MAX_ELEM_VETT]; /* Il massimo numero di  byte indirizzabili da un x386 e' di 4gb cioe'
 4294967296(4*1024*1024*1024).Ogni pagina e' di 4096 byte quindi il massimo numero di pagine e' 4294967296/4096 il cui risultato e'
 1048576. Ogni elemeno della bitmap corrisponde a 32 pagine; quindi per sapere il massimo numero degli elementi della bitmap bisogna
 fare 1048576/32 che fa proprio 32768 che e' il valore della define MAX_ELEM_VETT. Ricordo che non utilizzeremo tutti gli elementi, ma
 soltanto quelli che ci riguardano in base alla dimensione della memoria fisica. Se la RAM e' ad esempio di 128MB cioe' di 134217728byte
 le pagine disponibili saranno 134217728/4096 e cioe' 32768.  Il numero di elementi che ci servira' si otterra' facendo 32768/32 il cui
 risultato e' 1024 */

unsigned int max_pages_intomem;
unsigned int real_elem_vett;
unsigned int total_mem;
unsigned int status_kmalloc; /*Questa e' una variabile di stato che mi "dice" se la funzione alloc_pages e' stata chiamata dalla
kmalloc (in questo caso status_kmalloca vale 1) o meno (in questo caso status_kmalloca vale 0)*/

struct mem_pages_info info_pages_alloc; //questa e' la prima struttura che assegno alla lista 
struct mem_pages_info* list_info_pages; //puntatore di testa che deve puntare sempre al primo elemento della lista
struct mem_pages_info* tmp_list_info_pages; //conterra' temporaneamente l' indirizzo di un nuovo elemento creato

struct mem_pages_info* punt_first_struct_list; //utilizzeremo questo puntatore per puntare sempre alla prima struttura della 
										          //lista

 /********************************************************************************
********************************************************************************/
 /* Per stabilire la dimensione della memoria ram bisogna scrivere un valore in modo sequenziale(nella ram) e fermarsi fin quando
 non avviene un errore di scrittura, cioè quando non esistono piu locazioni. Sfruttando dunque un ciclo è possibile calcolare la
 dimensione
 della memoria RAM */
 
 //Calcola la dimensione della memoria RAM
void size_mem(void)
{
long flags;

		save_flags(flags); //salvo gli Eflags nella variabile flags attraverso la macro save_flags
		asm("cli"); //Disabilito gli interrupt

unsigned long int *mem_current = (unsigned long int *)0x100000; //Il calcolo viene effettuato dal primo Mega Byte della RAM


    outportb(0xFF, 0x21);                     //Disabilito gli interrupt hardware
    outportb(0xFF, 0xA1);

while(1)
{
*mem_current = (unsigned int)0xAA55AA55; //provo a scrivere un valore nella locazione di memoria corrente
           asm("": : :"memory");
if(*mem_current == (unsigned int)0xAA55AA55) // se riesco a leggerlo vuol dire che il valore è stato scritto
{
      //per sicurezza faccio un doppio controllo
    *mem_current= (unsigned long int)~0xAA55AA55;
     asm("": : :"memory");
     if(*mem_current == (unsigned long int)~0xAA55AA55) //se riesco a leggerlo vuol dire che il valore è stato scritto
     {
                 mem_current = mem_current+0x400; //aggiungo 1kb. Ricordo che 400 e' in decimale 1024
		 
}

else break; //altrimenti esce dal ciclo
}

else break; //altrimenti esce dal ciclo
  asm("": : :"memory");
}


total_mem=(unsigned long int)mem_current; //Valore in byte


		restore_flags(flags);  //Ripristino gli Eflags

}
/********************************************************************************
********************************************************************************/
//Inizializiamo le pagine cioe' poniamo tutti i bit della bitmap a 0 tranne i primi 256 che sono riservati al kernel e quindi devono risultare
//come occupati. Dunque i primi 8 elementi (256/32) dovranno essere posti come occupati.
void init_pages(void)
{
int i;
long flags;

		save_flags(flags); //salvo gli Eflags nella variabile flags attraverso la macro save_flags
		asm("cli"); //Disabilito gli interrupt


for(i=0; i<8; i++)
 bitmap[i]= 0xFFFFFFFF; //Pongo di volta in volta le 32 pagine che compongono un elemento come pagine  occupate

for(i=8; i<MAX_ELEM_VETT; i++)
 bitmap[i] = 0x00000000;  //Poniamo il resto delle pagine come pagine libere
 
 list_info_pages = &info_pages_alloc; //assegno al puntatore di testa il primo elemento(struttura) della lista
 punt_first_struct_list = list_info_pages; //ricordo che punt_first_struct_list deve puntare sempre alla prima struttura della lista
 
 //Al primo elemento del vettore di strutture che tiene conto delle informazioni delle pagine allocate assegniamo le informazioni
 //riguardanti lo spazio riservato al kernel
 list_info_pages->vett_pages_info[0].base_address = 0x00000000; //Questo e' l'indirizzo iniziale della memoria fisica che
 											    //naturalmente corrisponde all' indirizzo iniziale dove e' collocato
											   // il kernel
 list_info_pages->vett_pages_info[0].num_pages= 256;		   	//Ricordo che le prime 256 pag sono riservate al kernel
 list_info_pages->next_free_index = 1;							//prossimo elemento del vettore libero 
 list_info_pages->next = NULL;								//Inizializzo a NULL il puntatore alla prossima struttura
 
 
max_pages_intomem = total_mem/SIZE_PAGE; //E' il numero massimo di pagine in cui la memeria fisica della 															    //macchina su cui gira il sistema puo' essere divisa
										
real_elem_vett = max_pages_intomem/32;  //E' il numero massimo di elementi che ci servono all' interno della bitmap. 													//Ricordo che ogni elemento di 32 bit della bitmap equivalgono a 32 pagine
													//per questo si fa diviso 32
status_kmalloc = 0;


		restore_flags(flags);  //Ripristino gli Eflags

}
/********************************************************************************
********************************************************************************/
//Questa funzione si occupa di occupare num pagine libere all' interno della bitmap e di passare l' indirizzo fisico di partenza
void* alloc_pages(unsigned int num) //num e' il numero di pagine da "allocare" nella bitmap
{
int onebit, i , j=0, pag_continue=0, elem_scan;  /*la variabile 'j' tiene conto di bit di ogni elemento mentre la variabile 'i' tiene conto
proprio deglielementi della bitmap. */
unsigned long int address; 


if(status_kmalloc == 1) /* Se questa funziona ovvero alloc_pages è stata chiamata dalla funzione kmalloc(che alloca spazio riservato al kernel) allora la ricerca delle pagine nella bitmap deve cominciare dall' inizio della bitmap stessa */
{
 i = 0;
 status_kmalloc = 0;
 elem_scan = KERNEL_MEM_ELEMENTS; /*elem_scan indica fino a quando bisogna scansionare gli elementi nella bitmap. Nel caso in
  cui alloc_pages e' stata chiamata dalla funzione kmalloc gli elementi da scansionare per la ricerca delle pagine libere sono soltanto 
 gli elementi riservati al kernel e cioe' KERNEL_MEM_ELEMENTS */
}
else
{
 i=KERNEL_MEM_ELEMENTS; /*altrimenti se la funzione alloc pages NON e' stata chiamata dalla funzione kmalloc la ricerca delle
  pagine deve partire dalla fine dello spazio riservato al kernel */
 elem_scan = real_elem_vett; /* elem_scan indica fino a quando bisogna scansionare gli elementi nella bitmap. */
}
/* In pratica utilizzando la variabile di stato status_kmalloc faccio in modo che le allocazioni del kernel avvengano solo nel suo spazio
    riservato nella bitmap (Questo spazio viene deciso dalla define KERNEL_MEM in phisicmem.h). Mentre tutte le altre allocazioni nella
    bitmap vengono effettuate fuori dall' area riservata al kernel. */

   
 /* Ricordando tale concetto come esempio:  Si puo' utilizzare una stringa di bit particolare a mo' di maschera per 'vedere' solo alcuni bit. Ad
esempio, la serie di bit '00001111' e' una maschera binaria che puo' essere usata per 'vedere' i 4 bit piu' a destra (bit bassi) di un qualsiasi
byte. La serie di bit 00001111 corrisponde alla cifra decimale 15, percio' l'operazione di AND '25 & 15' corrisponde a 00011001 & 00001111 che produce come risultato 00001001:

00011001
&
00001111
=
00001001

*/
//Leggiamo il valore di ogni singolo bit della bitmap attraverso l' operatore AND (&) e shift a sinistra (<<)
 
 
for( ; i < elem_scan; i++) 
{
 while(j<32)   //j  tiene conto della posizione del bit sul quale viene applicata la maschera e quindi poiche' i bit di ogni elemento sono
 			 //32 j non puo'essere maggiore di 32
  {
   onebit =  bitmap[i] & (1 <<  j); //Leggo di volta in volta ogni singolo bit della bitmap
  
  if(onebit == 0) //se la pagina e' libera
 {
    pag_continue++; //incremento la variabile che tiene conto delle pagine continue libere
   
    if(pag_continue == num) //se sono state trovate num pagine continue libere
       break; //allora esci dal ciclo while
       
         j++; //incremento j che ricordo che tiene conto della posizione del bit sul quale viene applicata la maschera
    }
    
  else //altrimenti se la pagine e' occupata
    {
    j++;  //incremento j
   pag_continue = 0; //azzero count poiche' ricordo che le pagine devono essere continue e se una pagina e' occupata prima che count sia
    				   //uguale a num allora count deve azzerarsi e ripartire fin quando non si trovano num pagine continue
     }
     
  } //chiude il while
  
  if(pag_continue==num) //se sono state trovate num pagine libere 
  break; //allora dobbiamo uscire anche dal ciclo for
  
 j=0;  //azzero j perche' inizia la" scansione" di un nuovo elemento
} //chiude il for

if(pag_continue != num) //se non sono stati trovati num pagine libere
  return NULL; //ritorna NULL
  

//se sono state trovate num pagine libere allora mi calcolo l' indirizzo fisico di partenza
address = ((i*32+j+1)-pag_continue)*4096; 
//con (i*32+j+1) ci calcoliamo il numero di bit(pagine) finali dove e' stata trovata l' ultima pagina libera. Aggiungo 1 perche' se le pagine
//richieste sono 5,  j poiche' e' inizializzato a 0 alla fine varra' 4 e non 5.
//A queste togliamo le pagine trovate libere in modo da poterci calcolare solo la posizione della prima pagina trovata libera nella bitmap
//Poi moltiplichiamo per 4096 per trovarci l' indirizzo fisico 


//Ora dobbiamo rendere occupate le pagine richieste

i = (address/4096)/32;  //mi calcolo l' indice iniziale delle pagine allocate
j = (address/4096)%32; //mi calcolo i rimanenti bit iniziali delle pagine allocate


   pag_continue = 0; //utilizzo pag_continue come contatore ... giusto per non utilizzare altre variabili
  while(pag_continue < num) //fin quando il contatore e' minore delle pagine richieste
  {
  
      bitmap[i] = (bitmap[i] | (1<<j));  //pongo di volta in volta le pagine richieste come pagine occupate
      j++;							  //e incremento j
      pag_continue++;				//e incremento pagine_continue
      if(j==32 && pag_continue < num) //se j e' uguale a 32 
      {
          i++; //allora incremento l' indice nella bitmap
          j=0; //e azzero j che tiene conto dei singoli bit
      }
  }

//printk("Alloc: %d\n",bitmap[8]); //per debug
 
 add_info_pages((unsigned int)address, num); //chiamo la funzione che aggiunge ad un vettore di strutture le informazioni riguardanti
 										     //le pagine occupate
 
 
return (void*)address;  //Ritorno l' indirizzo fisico di partenza delle pagine richieste

}
/********************************************************************************
********************************************************************************/
//Questa funzione aggiunge ad un vettore di strutture le informazioni riguardanti le pagine occupate(indirizzo di base e numero di pagine)
void add_info_pages(unsigned int  address, unsigned int num)
{
int i, last;
list_info_pages =  punt_first_struct_list; //facciamo puntare il puntatore di testa alla prima struttura della lista

i=list_info_pages->next_free_index;

//controlliamo se nella lista ci sono strutture che hanno array(che contengono le info delle pagine) ancora NON completamente utilizzati
while(i > 510) //fin quando 'i' e maggiore di 510(cio' indica che non vi sono piu elementi dell' array disponibili nella struttura corrente)
  {

  //passiamo alle altre eventuali strutture della lista in modo da poter controllare se a sua volta hanno elementi dell' array ancora 
  //disponibili
     if(list_info_pages->next != NULL)
     {
       list_info_pages = list_info_pages->next;
       i=list_info_pages->next_free_index;
     }
     
     else 
      break;
  }


  if(i < 510) //Se tra le strutture che compongono la lista e'  stato trovalo un array ancora utilizzabile per metterci le informazioni 
  		    //riguardanti le pagine richieste
  {
  //allora proseguiamo per l' inserimento delle informazioni delle pagine richieste
    list_info_pages->vett_pages_info[i].base_address = (void*)address; 
    list_info_pages->vett_pages_info[i].num_pages = num;	   	
    list_info_pages->next_free_index++;		//prossimo elemento del vettore libero 


  }

 
   else //altrimenti, se NON e' strato trovato un array ancora utilizzabile per metterci le informazioni riguardanti le pagine richieste
  {
//dobbiamo introdurre un nuovo elemento(struttura) all' interno della lista cosi abbiamo a disposizione un' altra struttura con un altro 
//array di 510 elementi

//Importante: L' elemento verra' inserito all' inizio della lista quindi il puntatore di testa puntera' proprio al nuovo elemento e quest'
//ultimo puntera' al precedente elemento puntato dal puntatore di testa 

//I passaggi da eseguire sono i seguenti:
//1. Creare un esemplare della nuova struttura, allocando uno spazio di memoria mediante una funzione di allocamento
//dinamico(utilizzeremo la funzione alloc_pages)
//2. Associare al puntatore del nuovo elemento il valore attuale del puntatore di testa
//3. Assegnare al puntatore di testa l' indirizzo del nuovo elemento


    // 1)
   tmp_list_info_pages=(struct mem_pages_info*)alloc_pages(1);
   
	if(tmp_list_info_pages != NULL)
	 {
    // 2) 
   tmp_list_info_pages->next = list_info_pages;

    // 3)
   list_info_pages = tmp_list_info_pages;


//Ora che abbiamo creato il nuovo elemento(struttura) , list_info_pages(il puntatore di testa) punta ad una struttura "pulita"
   list_info_pages->vett_pages_info[0].base_address = (void*)address; 
   list_info_pages->vett_pages_info[0].num_pages = num;		   	
   list_info_pages->next_free_index = 1;		//prossimo elemento del vettore libero 

   punt_first_struct_list = list_info_pages; //ricordo che punt_first_struct_list deve puntare sempre alla prima struttura della lista che
   									     //in questo caso coincide con il nuovo elemento creato
          }
	  else
	    printk("\n- Impossibile creare un nuovo elemento nella lista: Memoria insufficiente ! -\n");
   
 } //chiude l' else iniziale
 
} //chiude la funzione
/********************************************************************************
********************************************************************************/
//Questa funzione si occupa della liberazione delle pagine occupate che diventeranno nuovamente libere
void dealloc_pages(void* address)
{
 int  i = ((int)address/4096)/32; //mi calcolo l' indice iniziale delle pagine allocate
 int  j = ((int)address/4096)%32; //mi calcolo i rimanenti bit iniziali delle pagine allocate
 int k = 0, num = 0, count=0, last;
struct mem_pages_info*  tmp_list;
struct mem_pages_info*  address_struct_list; //conterra l' eventuale indirizzo della struttura da eliminare nella lista 
  
  
 list_info_pages =  punt_first_struct_list; //facciamo puntare il puntatore di testa alla prima struttura della lista
 
 //scansioniamo le struttura della lista per poi ricavarci num_pages
for(k=0; list_info_pages->vett_pages_info[k].base_address != address; k++) 
{	
	//se nella lista abbiamo trovato l' indirizzo che desideriamo deallocare...
	if(list_info_pages->vett_pages_info[k].base_address == address)
	{	
		num = list_info_pages->vett_pages_info[k].num_pages;	//ci ricaviamo il numero di pagine da deallocare
		break;	//usciamo dal ciclo
	}
	else if(k == 510) /*altrimenti se ho scansionato tutto l' array e non ho trovato l' indirizzo che mi interessa allora
				  provo a vedere se nella lista ci sono altre strutture e di conseguenza un altro array
				 da scansionare*/
	{
		if(list_info_pages->next != NULL)	//Controllo se la prossima struttura da scansionare sia diversa da NULL
		{
			list_info_pages = list_info_pages->next;
			k=0; //inizializzo l' indice
			continue;
       		}
		else	//Se la prossima struttura e' NULL esco dal ciclo
			break;
	}
}

//Se non e' stato trovato l' indirizzo da deallocare stampiamo un messaggio e ritorniamo
if(list_info_pages->vett_pages_info[k].base_address != address)
{
	printk("\nAttenzione: l' indirizzo che si e' scelto di Deallocare non e' stato precedentemente Allocato");
	return;
}

 
//rendiamo libere le pagine che si devono deallocare all' interno della bitmap
while(count < num) //fin quando il contatore e' minore delle pagine che si devono rendere libere
    {
  
      bitmap[i] = (bitmap[i] ^ (1<<j)); //pongo di volta in volta le pagine occupate come pagine libere. Utilizzo l' operatore ^(xor). Ricordo che
      								 // l'operazione(con ^) restituisce valore vero ( uno ) Se e solo se uno dei due operandi è vero:
      								 //1 ^ 1 = 0 
      								 //1 ^ 0 = 1
								 //0 ^ 1 = 1
								 //0 ^ 0 = 0
      j++;			//e incremento j
      count++;		//e incremento count
      if(j==32 && count < num) //se j e' uguale a 32 
      {
          i++; //allora incremento l' indice nella bitmap
          j=0; //e azzero j che tiene conto dei singoli bit
      } //chiude l' if
	  
    } //chiude il ciclo while
       
//printk("\nFree: %d\n",bitmap[8]); //per debug
 
			/* Proseguiamo con "l' eliminazione" delle informazioni delle pagine che abbiamo reso libere */

     last =  list_info_pages->next_free_index - 1; //in last c' e' praticamente l' ultimo elemento utilizzato dall' array 
 

      if(last == 0 && list_info_pages->next != NULL) /*Se 'last' e' uguale a 0 elimino la struttura nella lista visto che le informazioni 
      delle pagine deallocate(liberate) non ci servono piu'. Controllo anche se list_info_pages e' diverso da NULL perche' se fosse 
      uguale a NULL si tratterebbe della struttura nel cui array, che tiene conto delle informazioni delle pagine allocate, all' indice 0
      sono state messe le informazioni riguardante la parte riservata al kernel e quindi la struttura non deve essere eliminata */
       {
       address_struct_list = list_info_pages;
       
       //Sistemo la lista senza la struttura che devo eliminare
       
       /* Ricorda:
       1) Per eliminare il primo elemento di una lista, assegnare al puntatore di testa l'indirizzo del secondo elemento
       
       2) Per eliminare un elemento intermedio x, assegnare al puntatore prossimo dell' elemento che precede x l'indirizzo dell'
            elementeo che segue x
*/
      		// 1)
	        if(list_info_pages == punt_first_struct_list)
	        {
		  list_info_pages = list_info_pages->next;
		}
		
	        // 2)
		else
		 { 
		  tmp_list=punt_first_struct_list;
		  
		//mi calcolo quale elemento precede la struttura che voglio eliminare
	           while(tmp_list->next != NULL)
	             {
		     
		      if(tmp_list->next == list_info_pages)
		        break;
		    
		      else
		        tmp_list=tmp_list->next;
		       
	              }
		      //sistemo la lista senza l' elemento intermedio che voglio escludere
		     tmp_list->next = list_info_pages->next;
		 }
	
      
 		        //dealloco la struttura in memoria
		       //ricordo che l' allocazione di una struttura ha richiesto UNA sola pagina
     		  int  i = ((int) address_struct_list/4096)/32; //mi calcolo l' indice iniziale della pagina allocata
   	      	  int  j = ((int)address_struct_list/4096)%32; //mi calcolo i rimanenti bit iniziali della pagina allocata
       
		 bitmap[i] = (bitmap[i] ^ (1<<j));  //pongo la pagina occupata come pagina libera
   
	} //chiude l' if iniziale
      
      else   //altrimenti scaliamo tutto di un posto visto che le informazioni delle pagine deallocate(liberate) non ci servono piu'
       {
     
        while(k < list_info_pages->next_free_index)
     	    {
      
		list_info_pages->vett_pages_info[k].base_address =  list_info_pages->vett_pages_info[k+1].base_address;
		list_info_pages->vett_pages_info[k].num_pages =  list_info_pages->vett_pages_info[k+1].num_pages;	
    
    		  k++;
    
   	   }
       //Poiche' abbiamo scalato tutto di un posto bisogna decrementare la variabile next_free_index
       list_info_pages->next_free_index--;
       }
}
