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
* Descrizione: Definizioni di funzioni per l' inizializzazione e gestione dello scheduler
*******************************************************************************************************/
 
 

#include<scheduler.h>
#include<task.h>
#include<gdt.h>
#include<system.h> //Per le macro save_flags e restore_flags

pid_t free_pid;  //sara' sempre uguale al prossimo pid disponibile
pid_t pid_cur_task; //sara' sempre uguale al pid del processo corrente
task_t process_table[MAX_TASK]; //e' la tabella dei processi

char scheduler_active; //stato dello scheduler: Abilitato = 1 , Disabilitato = 0
struct x86_TSS *TSS;


/* Assegna il tempo processore al task.*/
/* Un quanto e' una porzione di tempo che viene data ad un processo.
Al termine del quanto(quantum == 0) lo scheduler "blocca" il processo corrente mettendolo in
stato READY e seleziona un altro task da mandare in esecuzione (stato RUNNING). Lo scheduler, 
una volta scelto il task da mandare in esecuzione, chiama la funzione sottostante per 
inizializzare/assegnare una porzione di tempo di esecuzione, al termine del quale il task sara' 
rimesso in stato READY.
Notare che ai processi creati con privilegio SUPERVISOR viene data una maggiore quantita' 
di tempo, poiche' si presuppone che siano piu' importanti di quelli creati con privilegio USER. */
static void set_quantum(pid_t pid)
{
	if(process_table[pid].privilege == SUPERVISOR_PRIVILEGE)
		process_table[pid].quantum = 10;
	else
		process_table[pid].quantum = 8;
}	

/********************************************************************************
********************************************************************************/
//Inizializza lo scheduler
 void init_scheduler()
{
int i=0;
long flags;
free_pid=0;
pid_cur_task=0;
scheduler_active=0;

	save_flags(flags); //salvo gli Eflags nella variabile flags attraverso la macro save_flags
	asm("cli"); //Disabilito gli interrupt
	
	//Inizializzo la tabella dei processi mettendo lo stato di ciascun alemento a KILLED
	while(i<MAX_TASK) {
		process_table[i].state = KILLED;
		i++;
	}
	
	create_task(idle_task, "Idle", SUPERVISOR_PRIVILEGE); //la funzione idle_task e' definita in task.c
	
	restore_flags(flags);  //Ripristino gli Eflags
}
/********************************************************************************
********************************************************************************/
//La funzione sottostante decide quale task mandare in esecuzione
void scheduler(unsigned int* cur_stack)
{
unsigned long task_pd_addr;
unsigned char old_privilege;

  if (get_scheduler() == 1) //Controllo se lo scheduler e' attivo
	{		
		
		/* Se il tempo processore non e' scaduto, ritorna */
		if(process_table[pid_cur_task].quantum--)
			return;

		
		 //Salvo lo STACK CORRENTE nel campo esp del task corrente
			process_table[pid_cur_task].esp = *cur_stack;
		
			if(process_table[pid_cur_task].state == RUNNING)
		 		process_table[pid_cur_task].state = READY;
		
		 
		 //L' algoritmo di scheduling utilizzato e' il Round Robin. 
			round_robin();		/*sceglie il nuovo task da mandare in esecuzione. Il pid di tale task e' memorizzare nella
			 				  variabile globale pid_cur_task*/
	
		/* Assegna il tempo processore al processo */
		set_quantum(pid_cur_task);		
		
		*cur_stack = process_table[pid_cur_task].esp; //Quando assegno un valore ad un parametro di una funzione questo valore
										//viene "automaticamente" messo sullo stack corrente. Cio' serve per popare
										//(riprendere) dallo stack corrente il valore del nuovo stack del processo da
										//mandare in esecuzione. Questo valore viene popato nel file idt.c attraverso
										//la funzione handler_asm attraverso le seguenti istruzioni in asm:
										//"popl %eax \n" 
										//"mov %eax, %esp \n"

		process_table[pid_cur_task].state = RUNNING; //poiche' il processo andra' in esecuzione lo metto in stato RUNNING
		
		 /* Ad ogni cambio di contesto cambio anche la PageDir per mettere la PageDir del processo che andra' in esecuzione  */

		
		/* Imposto TSS->ESP0 con il kernel stack del task che andra' in esecuzione. Per informazioni sulla TSS si veda il file gdt.h e
		 gdt.c */
		TSS->ESP0 = (unsigned int)process_table[pid_cur_task].kernel_stack;

		
		disable_paging();
 		task_pd_addr = (unsigned long int)process_table[pid_cur_task].PDBR & 0xFFFFF000; //Il valore da inserire nel registro cr3
 		asm volatile("movl %0, %%cr3\n" : : "r"(task_pd_addr)); //Inserisco il valore nel registro cr3
		enable_paging();
		
		
		disable_scheduler(); //disabilito lo scheduler
		

	}	
		
	
	
}
/********************************************************************************
********************************************************************************/
void round_robin(void)
{

	 /*Controllo se ci sono processi in stato READY da essere eseguiti. Appena ne trovo uno esco dal ciclo. Se non ne trovo
	 allora pid_cur_task viene azzerato. Ricordo che pid_cur_task(cosi come free_pid) e' globale quindi non e' necessario ritornare
	  alcun valore, infatti, pid_cur_task sara' visibile da qualsiasi funzione.*/
	do
	{
	 pid_cur_task++;
	
		if(pid_cur_task >= free_pid)
		  pid_cur_task = 0;
		  
	} while (process_table[pid_cur_task].state != READY);
	
}
/********************************************************************************
********************************************************************************/
//Abilita lo scheduler
void enable_scheduler(void)
{
	scheduler_active = 1;
}
/********************************************************************************
********************************************************************************/
//Disabilita lo scheduler
void disable_scheduler(void)
{
	scheduler_active = 0;
}
/********************************************************************************
********************************************************************************/
//Ritorna il valore dello stato dello scheduler(0 o 1)
int get_scheduler(void)
{
	return scheduler_active;
}
