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

/***********************************INFO***********************************
* Coders: Black
* Descrizione: Definizioni di funzioni per la gestione dei tasks
*****************************************************************************/ 
 

 
#include<task.h>
#include<phisicalloc.h>
#include<io.h>
#include<paging.h>
#include<string.h>
#include<system.h>	//per le macro save_flags e restore_flags
#include<syscall.h>


pid_t free_pid;  //sara' sempre uguale al prossimo pid disponibile
pid_t pid_cur_task; //sara' sempre uguale al pid del processo corrente
task_t process_table[MAX_TASK]; //e' la tabella dei processi

/* Current console: */
struct console_s *cur_console; //global

struct stack_struct stack_info;
/********************************************************************************
********************************************************************************/
 //Questa funzione si occupa di creare nuovi processi
pid_t create_task(void (*func)(), char* name, unsigned char privilege)
 {

 
unsigned long *task_page_table, *stack_page_table;
void *kstack_physic_addr, *ustack_physic_addr;
unsigned long address = 0;
unsigned int i;
unsigned int* stacksetup, ustack;
long flags;
	
	save_flags(flags); 	//salvo gli eflags attraverso la macro save_flags. Il valore attuale degli eflags verra' messo in 'flags'
        asm("cli"); //disabilito gli interrupt
	
	
	disable_paging(); //Disabilito la paginazione		
	
/* Ogni task deve avere il suo spazio di indirizzamento virtuale , per questo ad OGNI task do una propria PageDir */

	
/* Poiche voglio che ogni pocesso abbia la sua PageDir alla creazione di un nuovo task alloco una nuova PageDir e una nuova
PageTable. La dimensione della PageDir e PageTable e' di una pagina cioe' di 4096 byte */
	process_table[free_pid].PDBR = (unsigned long *)phisic_alloc(4096);
	task_page_table = (unsigned long *)phisic_alloc(4096);
	
	//stack_page_table = (unsigned long *)phisic_alloc(4096);
	
	

/* Nei primi 4Mb della PageDir si deve mappare il kernel in modo che l' indirizzo virtuale sia proprio uguale all' indirizzo fisico */


//inizializzo la page table
for(i=0; i<1024; i++)
{

	task_page_table[i] = address | USER_PAGES_FLAG;
	address = address + 4096;
}

//Assegno alla PageDir di indice 0 l' indirizzo della page table
process_table[free_pid].PDBR[0] = (unsigned long)task_page_table;
process_table[free_pid].PDBR[0] = process_table[free_pid].PDBR[0] | USER_PAGES_FLAG;

/* Setto tutte le altre entry della PageDir come non presenti. Per fare questo basta impostare solo il flag di lettura e scrittura */
for(i=1; i<1024; i++)
  process_table[free_pid].PDBR[i] = 0 | READ_WRITE;
  
   /* All' ultimo elemento della PageDir faccio puntare la PageDir stessa */ 
  process_table[free_pid].PDBR[1023] = (unsigned long)process_table[free_pid].PDBR;
  process_table[free_pid].PDBR[1023] = process_table[free_pid].PDBR[1023] | USER_PAGES_FLAG;
  
  
  
  /*
  
 for(i=0; i<RESERVED_STACK_PAGES; i++)
{

	 	if( i == BLANK_PAGE_1 ) 
		{	//Se i == BLANK_PAGE_1 significa che ci troviamo all' inizio dello stack
			stack_info.start_stack_virtual_addr = make_virtual_address(1, i, 0);
			stack_info.blank_addr_1 = make_virtual_address(1, i, 0);
		}
			
		
		else if( i == BLANK_PAGE_2 )
			stack_info.blank_addr_2 = make_virtual_address(1, i, 0);
			
			
		else if( i == BLANK_PAGE_3 ) 
		{	//Se i == BLANK_PAGE_3 significa che ci troviamo alla fine dello stack
			stack_info.end_stack_virtual_addr = make_virtual_address(1, i, 0);
			stack_info.blank_addr_3 = make_virtual_address(1, i, 0);
		}
	
	
		else if(i == LOW_USER_STACK_PAGE)
			{
			stack_info.low_stack_user_addr =  make_virtual_address(1, i, 0) + 4096;
			ustack_physic_addr = phisic_alloc(4096);
			stack_page_table[i] = (int)ustack_physic_addr | USER_PAGES_FLAG;
			continue;
			}
			
		else if(i == LOW_KERNEL_STACK_PAGE)
			{
			stack_info.low_stack_kernel_addr =  make_virtual_address(1, i, 0) + 4096;
			kstack_physic_addr = phisic_alloc(4096);
			stack_page_table[i] = (int)kstack_physic_addr | KERNEL_PAGES_FLAG;
			continue;
			}
			
		//Tutti gli indirizzi soprastanti mi serviranno soprattutto per gestire eventuali pagefault (per stack insufficiente)
		
	stack_page_table[i] = 0;
}
*/
  
//process_table[free_pid].PDBR[1]  = (unsigned long)stack_page_table;
 
//process_table[free_pid].esp0 = (int)stack_info.low_stack_kernel_addr; //assegno l' indirizzo virtuale dell' inizio del kernel stack

// process_table[free_pid].tss = (tss_t*)((unsigned int)(kstack_physic_addr + 4096) - 100);	 //Creo lo stack e assegno l'
	 															//indirizzo di partenza
			


	/*In BeWos ogni task ha un kernel stack e, se e' stato creato con privilegio USER, avra' anche un user stack */

	//---------alloco il kernel stack---------
	process_table[free_pid].kernel_stack = (unsigned int*)kmalloc(4096);
	process_table[free_pid].init_kstack = process_table[free_pid].kernel_stack;
	

	process_table[free_pid].kernel_stack += 4096; /*siccome abbiamo allocato 4096 byte e trattandosi di un indirizzo che
	 utilizzeremo come uno stack, aggiungo 4096 all' indirizzo, in modo da puntare alla fine dell' allocazione. Ricordo,infatti, che
	  lo stack e' una Pila (Lifo).*/

	 //---------------------------------------
	 
	 
	 
/****************** INIZIALIZZAZIONE DEL KERNEL STACK ******************/

	stacksetup = process_table[free_pid].kernel_stack;

	*stacksetup--;

	if(privilege == USER_PRIVILEGE) //Se il privilegio e' quello USER 
	{
	
	//---------alloco l' user stack---------
	process_table[free_pid].user_stack = (unsigned int*)kmalloc(4096); //alloco l' user stack
	process_table[free_pid].init_ustack = process_table[free_pid].user_stack; 
	
	process_table[free_pid].user_stack += 4096; /*siccome abbiamo allocato 4096 byte e trattandosi di un indirizzo che
	 utilizzeremo  come uno stack, aggiungo 4096 all' indirizzo, in modo da puntare alla fine dell' allocazione. Ricordo,infatti, che
	  lo stack e' una Pila (Lifo).*/
	
	//metto sull' user stack l' indirizzo di "fine task". Quando il task terminera' il controllo pessera' alla funzione end_task.
	*process_table[free_pid].user_stack--;
	*process_table[free_pid].user_stack = (unsigned)end_task; /*l' indirizzo di questa funzione e' l' indirizzo di return. Quando si
	 ritornera' dal processo, il controlllo passera' alla funzione messa sull' user_stack. in pratica puo' essere definita come la
	  funzione chiamante*/
	//-----------------------------------------
	
	
	/*sul kernel stack metto anche i valori di ss e dell' esp dell' user. infatti quando si entra in user mode la iret ripristina dallo stack
	 anche tali valori*/
	*stacksetup-- = USER_DATA_SELECTOR; //ss3
	*stacksetup-- = (unsigned int)process_table[free_pid].user_stack; //esp3

	}
	 
	else
	//metto sul kernel stack l' indirizzo di "fine task". Quando il task terminera' il controllo pessera' alla funzione end_task
	*stacksetup--= (unsigned)end_task; /*l' indirizzo di questa funzione e' l' indirizzo di return. Quando si ritornera' dal processo, il
	controlllo passera' alla funzione messa sullo stack. in pratica puo' essere definita come la funzione chiamante*/


        *stacksetup--=0x202; //inizializzo gli eflag col valore di default 0x202
	
	if(privilege == SUPERVISOR_PRIVILEGE)
           *stacksetup--=KERNEL_CODE_SELECTOR;  //Inizializzo il registro di sistema cs(il selettore di segmento)
	else
	    *stacksetup--=USER_CODE_SELECTOR;	  //Inizializzo il registro di sistema cs(il selettore di segmento)
	
	    
        *stacksetup--=(unsigned int)func; //Inizializzo il registro di sistema eip
	
        *stacksetup--=0;    //ebp
        *stacksetup--=0;    //esp
        *stacksetup--=0;    //edi
        *stacksetup--=0;    //esi
        *stacksetup--=0;    //edx
        *stacksetup--=0;    //ecx
        *stacksetup--=0;    //ebx
        *stacksetup--=0;    //eax
	
	if(privilege == SUPERVISOR_PRIVILEGE)
	{
        *stacksetup--=KERNEL_DATA_SELECTOR; 	//ds
        *stacksetup--=KERNEL_DATA_SELECTOR; 	//es
        *stacksetup--=KERNEL_DATA_SELECTOR; 	//fs
        *stacksetup=  KERNEL_DATA_SELECTOR; //gs
	}
	
	else
	{
	*stacksetup--=USER_DATA_SELECTOR; //ds
        *stacksetup--=USER_DATA_SELECTOR; //es
        *stacksetup--=USER_DATA_SELECTOR; //fs
        *stacksetup=  USER_DATA_SELECTOR; //gs
	}	
	
/****************** FINE INIZIALIZZAZIONE DEL KERNEL STACK ******************/
	


	process_table[free_pid].esp = (unsigned int)stacksetup;	/* Inizializzo il campo esp(che tiene conto dello stack corrente) con lo
	 stack kernel che abbiamo appena creato ed inizializzato. */
	 
	process_table[free_pid].privilege = privilege;
	
	
	process_table[free_pid].pid = free_pid; //assegno al campo ''pid' il pid del processo creato
	process_table[free_pid].ppid = pid_cur_task; //assegno al campo ''pid' il pid del processo creato
	process_table[free_pid].state = READY; //metto il task in stato READY quindi e' pronto per l' esecuzione
	strcpy(process_table[free_pid].name, name); //assegno al campo 'name' il nome del task
	
	process_table[free_pid].console = cur_console; /* inizializzo il campo console con la console nel quale e' stato creato il task(cioe'
	 con la console corrente. Per maggiori info si veda task.h */
	/* Inizializza a zero il quantum del processo */
	process_table[free_pid].quantum = 0;

	
	//printk("Task '%s' creato con PID %d",process_table[free_pid].name, free_pid);
	free_pid++; //incremento il prossimo pid disponibile
	
	
	enable_paging(); //riabilito la paginazione
	
	
	restore_flags(flags);	//ripristino gli eflags attraverso la macro restore_flags
	
	return process_table[free_pid-1].pid; //restituisco il pid del processo creato
	
}
/********************************************************************************
********************************************************************************/
//Questa funzione viene chiamata al termine di un processo
void end_task(void)
{
//asm("cli");
	kill_process(pid_cur_task);	//chiamo la syscall che uccide il processo con pid "pid_cur_task"
	
	//printk("cazzo");
//asm("sti");
	while(1);
}
/********************************************************************************
********************************************************************************/
//Il task idle e' il primo task che deve essere presente all' interno della process table. Esso viene inserito nella process table in fase di
//inizializzazione dello scheduler nel file scheduler.c ed ha sempre pid 0
void idle_task(void)
{
while(1);
}
/********************************************************************************
********************************************************************************/
//Ritorna il pid del task correntemente in esecuzione
int get_current_pid_task(void)
{
return pid_cur_task; //pid_cur_task e' sempre uguale al pid del processo corrente
}
