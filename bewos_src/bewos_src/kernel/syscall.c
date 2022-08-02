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
* Coders: Black
* Descrizione: Definizioni delle System Calls e di funzioni per la gestione di esse
***********************************************************************************************/



#include <syscall.h>
#include <gdt.h>
#include <task.h>
#include <scheduler.h>
#include <delay.h>
#include <keyboard.h>
#include <scank.h>
#include <io.h>
#include <system.h> //per save_flags e restore_flags
#include <console.h>
#include <vfloppy.h>
#include <timer.h>

int errno;

char buffer_kbd[BUF_SIZE]; //global. Buffer contenente i caratteri digitati sulla keyboard. Mi serve per la syscall sys_getche()
unsigned int pos_buffer_kbd; //global. Indice corrente del buffer_kbd



//----- Funzioni di gestione delle System Calls -----

void system_call_handler(void)
 {

    __asm__ volatile (".globl system_call_handler  \n"
    
    	"system_call_handler: \n"
	
    //Controllo se in eax c' e' un id valido. l' istruzione cmpl controlla se %eax > NR_syscalls . Se e' cosi chiama syscall_not_found
	"cmpl %0, %%eax \n" 
	"ja	syscall_not_found\n"
	//cld
	"push	%%ds \n" //pusho il registro di segmento dati
	"push	%%es \n" //pusho il registro es

/*pusho gli argomenti della syscall. Ricordo che gli argomenti della syscall sono stati messi nei registri generali(che di seguito pusho
 sullo stack) dalla wrapper routine*/
	"pushl	%%ebp \n"
	"pushl	%%edi \n"
	"pushl	%%esi \n"
	"pushl	%%edx \n"
	"pushl	%%ecx \n"
	"pushl	%%ebx \n"
//Aggiorno i registri di segmento con il descrittore kernel data
	"movw	%1, %%bx \n"
	"movw	%%bx, %%ds \n"
	"movw	%%bx, %%es \n"
//abilito gli interrupt cosi' una syscall puo' essere sospesa da qualche altro interrupt!
	"sti\n"	
	// Eseguo la system call
	"call	*system_call_table(,%%eax,4) \n"
// una volta terminata la syscall bisogna ripristinare i registri
	"popl		%%ebx \n"
	"popl		%%ecx \n"
	"popl		%%edx \n"
	"popl		%%esi \n"
	"popl		%%edi \n"
	"popl		%%ebp \n"
	"pop		%%es \n"
	"pop		%%ds \n"
	"iret \n"
	"\n" :: "i" (NR_syscalls), "i" (KERNEL_DATA_SELECTOR));

}


void syscall_not_found(void)
{
printk("\nAttenzione: La syscall specificata non esiste\n");

asm("movl	$-1, %eax");
asm("iret");
}


void setup_syscall_table(void* func, unsigned int syscall_number)
{

 if(syscall_number >= 0 && syscall_number < NR_syscalls) //verifico che il syscall_number sia valido
	system_call_table[syscall_number] = func;
	
 else
   {
	printk("\n\nNumero di Syscall Non valido. Specificare un numero >= 0 e < %d\n\n", NR_syscalls);
	return;
   }
   
}


void init_sys_call(void)
{
setup_syscall_table(sys_debug, __NR_debug);
setup_syscall_table(sys_getpid, __NR_getpid);
setup_syscall_table(sys_sleep, __NR_sleep);
setup_syscall_table(sys_getche, __NR_getche);
setup_syscall_table(sys_puts, __NR_puts);
setup_syscall_table(sys_fork, __NR_fork);
setup_syscall_table(sys_kill_process, __NR_kill_process);
setup_syscall_table(vfloppy_rd, __NR_vfloppy_raw_read);
setup_syscall_table(vfloppy_wt, __NR_vfloppy_raw_write);
setup_syscall_table(sys_do_timer, __NR_do_timer);
setup_syscall_table(sys_do_sem, __NR_do_sem);
}





//------ System Calls ------


int sys_debug(int number, int number2)
{
printk("\n\nSystem Call di Debug. Parametri: %d - %d\n\n", number, number2);
//asm("push %cs\n");
}


int sys_getpid(void)
{
return get_current_pid_task();
}


int sys_sleep(unsigned long ms)
{
delay(ms);

return 0;
}



int sys_getche()
{
int i;
long flags;
unsigned temp;


	unpress_enter();
	unpress_key();
	
	abilita_kbd();
    
    while( get_press_key() == 0 && get_press_enter() == 0 );

	disabilita_kbd();
	
	unpress_key();
	unpress_enter();
    
	if( buffer_kbd[pos_buffer_kbd-1] == '\n' )
		printk("\n");
		
	else if( buffer_kbd[pos_buffer_kbd-1]  == '\b')
	{

		if(cur_console->pos_video == cur_console->block_videomem ) /*Faccio questo controllo per controllare se ci troviamo in
		 un punto dove non si deve cancellare. Se non utilizzassimo questo if l' utente potrebbe cancellare tutta la memoria video!! */
			return '\b';  //ritorno solo \b, ma NON cancello il carattere dalla memoria video


		cur_console->pos_video-=2; //Vado indietro di un carattere. faccio videomem-=2 perch�bisogna contare anche il colore
		cur_console->x--; //sono andato indietro di un carattere, quindi decremento la variabile x
		if(cur_console->x == 0) //se cancelliamo tutta la riga ...
		{
			cur_console->y--; //decremento la variabile che tiene conto delle righe
			cur_console->x = SCR_C; //setto la variabile x a SCR_C perche' ci troviamo alla fine della riga precedente
		}

		/*stampo uno spazio in modo che sovrapponendosi al carattere che vogliamo cancellare si ha la sensazione della
		 cancellazione*/
		*(char*)cur_console->pos_video = ' ';
		*(char*)(cur_console->pos_video+1) = cur_console->attr;

		set_cursor_pos(); //aggiorno il cursore

		}

	
		
		else 
			printk("%c", buffer_kbd[pos_buffer_kbd-1]); //Stampo il carattere


return (char)buffer_kbd[pos_buffer_kbd-1];	//ritorno il carattere all' interno del buffer
 
}


int sys_puts(const char *s)
{

long flags;

save_flags(flags);	//la macro save_flags mette il valore corrente degli Eflags nella variabile flags
asm("cli"); /*dopodiche' disabilito gli interrupt(cosa succederebbe se mentre stampiamo una stringa qualcuno preme un tasto della
 tastiera ?.. ovviamente si accavallerebbero i caratteri digitati sulla tastiera e quelli che devono essere stampati dalla printk e questo
  deve essere evitato. Per questo ed altri motivi, quando stampiamo caratteri a video, disabilitiamo gli interrupt ! )*/
  

pid_t pid_current_task = get_current_pid_task();	//alla variabile assegno il pid del task corrente
unsigned int i;


    
  //pid_current_task = 0 equivale al task idle ed e' riservato
      if(pid_current_task != 0 && process_table[pid_current_task].console != cur_console) /*Siccome l' output di un task deve essere
       stampato nella console nel
     quale esso stato creato, controllo se la console(nella quale e' stato creato il task) e' diversa dalla console corrente*/
    {/*Se e' cosi', memorizzo nel buffer della console nella quale e' stato creato il task, cio'  che il task dovrebbe stampare a video.
     Ricordo, infatti, che se un task viene creato nella console numero 1, allora il task deve stampare a video solo nella console
      numero 1.
      Se non ci troviamo nella console numero 1, cio' che dovrebbe essere visualizzato a video viene salvato nel buffer della console
       numero 1 e il buffer verra' stampato quando si ri-switchera' alla console numero 1. Per maggiori info si veda il file console.h e
        console.c */
       
	i = process_table[pid_current_task].console->index_buffer;	//metto in 'i' l' indice corrente del buffer
    	for( ; *s !=0 ; s++)
	{
		//Controllo se e' stata superata la dimensione del buffer; se e' cosi' azzero tutto e continuo a ciclare
		if(i >= SCR_C*SCR_R)
		{
			memset(cur_console->buffer_temp, 0, SCR_C*SCR_R);  //azzero il buffer
			cur_console->index_buffer = 0;  //azzero l' indice del buffer
			i = 0;
			continue;
		}
		
		
		process_table[pid_current_task].console->buffer_temp[i++] = *s; //memorizzo il carattere nel buffer e incremento l' indice
	}	

		
	process_table[pid_current_task].console->index_buffer = i;	//inizializzo l' indice del buffer con l' indice corrente
	process_table[pid_current_task].console->buffer_temp[i]=0;	//chiudiamo il buffer
	restore_flags(flags);
	return 0;     
	}  
  
  //Altrimenti Stampo a video la stringa carattere per carattere
    for (; *s; s++)
        putk(*s); //putk definita in printk.c

restore_flags(flags); //ripristino gli Eflags al valore salvato precedentemente prima della disabilitazione degli interrupt


/*Utilizziamo save_flags e restore_flags anzich�le istruzioni "cli" e successivamente "sti" per disabilitare/abilitare gli interrupt. Questo avviene per una ragione molto importante. Supponiamo di invocare printk da una routine che ha disabilitato gli interrupt per qualche motivo. Supponiamo, inoltre, di usare l' istruzione "cli" e successivamente "sti" all'interno di printk anzich�le routine save_flags e restore_flags. Il risultato sarebbe che all'uscita della printk gli interrupt vengono abilitati, mentre quello che si vorrebbe �di ritornare alla routine chiamante con lo stato del bit IF(che detto banalmente tiene conto se gli interrupt sono disabilitati o meno) del registro EFLAGS uguale a prima dell'invocazione.*/

return 0;
}


/*Ovviamente questa funzione e' temporanea e si limita solo a creare nuovi task. In seguito verra' fatta la fork vera e
 propria(processi padre, figli ecc..) */
int sys_fork(void (*func)(), char* name, unsigned char privilege)
{

	create_task(func, name, privilege);
	
}



//"Uccide" il processo con pid 'pidnum'
void sys_kill_process(pid_t pidnum)
{
	//int i=pidnum;
long flags;

	save_flags(flags);	//la macro save_flags mette il valore corrente degli Eflags nella variabile flags
	asm("cli"); 		//disabilito gli interrupt

	process_table[pidnum].state 		=   	KILLED;
//	process_table[pidnum].esp 		= 		0;		
//	strcpy(process_table[pidnum].name, NULL);
//	process_table[pidnum].pid 		= 		0;
//	process_table[pidnum].privilege 	= 		0;

	//phisic_free(process_table[pid_cur_task].init_kstack);
	//phisic_free(process_table[pid_cur_task].init_ustack);

	
	
	restore_flags(flags);	//ripristino gli Eflags con la macro restore_flags
	/*
	//Poiche' un processo e' stato killato, scalo tutto di un posto all' interno della process table
	while(i<free_pid-1)
	 {
	  process_table[i].state =  process_table[i+1].state
	  process_table[i].esp 	=  process_table[i+1].esp 	
	  process_table[i].name = process_table[i+1].name	
	  process_table[i].pid 	=  process_table[i+1].pid-1
	  
	  i++;
	  }	

  free_pid--; //decremento la variabile che tiene conto del prossimo elemento libero nella processo table
*/
}
