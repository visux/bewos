#include<gdt.h>
#include<paging.h>
#include<console.h>

#ifndef __TASK_H
#define __TASK_H


#define MAX_TASK 100
#define MAX_LEN 50
#define MAX_NAME_LEN 50
#define STACK_SIZE 0x1000

#define RUNNING 0 //Stato Running: il processore e' assegnato al processo (dal S.O.) che puo' avanzare nell'esecuzione.
#define READY 1    //Stato Ready: il processo e' in attesa dell'assegnazione del processore, che e' occupato da un altro processo.
#define BLOCKED 2 //Stato Blocked: il processo non dispone del processore perche' bloccato in attesa di un servizio.
#define KILLED 3    //Stato KILLED: il processo e' stato killato




/* Define riguardanti lo Stack */


/* La regione dello stack deve avere nella PageTable un aspetto simile:

blank  --  high user stack pages  --  low user stack pages  --  blank  --  high kernel stack pages -- low kernel stack pages  --  blank

Per blank si intende una pagina not-present. Solo la parte "low" e' PRESENT
*/


#define USER_STACK_SIZE		128		//Dimensione in KB
#define KERNEL_STACK_SIZE		8		//Dimensione in KB

#define TOTAL_STACK_SIZE	 USER_STACK_SIZE + KERNEL STACK_SIZE


#define USER_STACK_PAGES 		USER_STACK_SIZE / PAGE_SIZE
#define KERNEL_STACK_PAGES 	KERNEL_STACK_SIZE / PAGE_SIZE

  /*Nella define sottostante aggiungiamo 3 poiche' sono le pagine di Blank che delimitano l' user stack e kernel stack*/
#define RESERVED_STACK_PAGES	USER_STACK_PAGES + KERNEL_STACK_PAGES + 3

#define BLANK_PAGE_1   0 								//Primo Blank
#define BLANK_PAGE_2   USER_STACK_PAGES + 1				//Secondo Blank
#define BLANK_PAGE_3   RESERVED_STACK_PAGES			//Terzo Blank
//BLANK_PAGES_3 indica l' ultima pagina dello stack; per questo gli assegno proprio le pagine totali riservate allo stack 


#define LOW_USER_STACK_PAGE		BLANK_PAGE_2 - 1
#define LOW_KERNEL_STACK_PAGE		BLANK_PAGE_3 - 1

/*------------------------------------------------*/


struct stack_struct
{
void* start_stack_virtual_addr;	//conterra' l' indirizzo dell' inizio dello stack
void* end_stack_virtual_addr;	//conterra' l' indirizzo della fine dello stack

void* low_stack_user_addr;		//conterra' l' indirizzo della parte low dell' user stack
void* low_stack_kernel_addr;	//conterra' l' indirizzo della parte low del kernel stack

void* blank_addr_1;			//conterra' l' indirizzo della parte blank_1 dello stack
void* blank_addr_2;			//conterra' l' indirizzo della parte blank_2 dello stack
void* blank_addr_3;			//conterra' l' indirizzo della parte blank_3 dello stack

};

extern struct stack_struct stack_info;


/*********************************/


typedef unsigned int pid_t; //definisco il tipo di dato pid_t


/* Task State Segment */
typedef struct {

	unsigned int ds;
	unsigned int es;
	unsigned int fs;
	unsigned int gs;

	unsigned int edi;
	unsigned int esi;
	unsigned int ebp;
	unsigned int null;
	unsigned int ebx;
	unsigned int edx;
	unsigned int ecx;
	unsigned int eax;
	

	unsigned int eip;
	unsigned int cs;
	unsigned int eflags;
	unsigned int* esp3; //aggiunto
	unsigned int ss3; //aggiunto
	unsigned int fine;
} tss_t;



//Quella che segue e' la struttura dati di un processo
typedef struct
{
	/*Variabili di Stack*/
	unsigned int esp;			//conterra' lo stack corrente
	unsigned int* kernel_stack;
	unsigned int* user_stack;

	unsigned int* init_kstack; //conterra' l' indirizzo iniziale del kernel stack. Utilizzero' questo indirizzo solo per deallocare lo stack
	unsigned int* init_ustack; //conterra' l'indirizzo iniziale dell' user stack. Utilizzero' questo indirizzo solo per deallocare lo stack
	/* ----*/
	
	pid_t pid; //il pid del task
	pid_t ppid; //il pid del task parent
	
	char name[MAX_NAME_LEN]; //Nome del Task
	unsigned long* PDBR; //Page Dir Base Register. Contiene l' indirizzo della pagedir del processo
	
	unsigned char privilege;  //Privilegio del processo (Supervisor, User)
	unsigned char state;	   //Stato del task
	unsigned int quantum;	//Tempo processore
	
	struct console_s* console;	/* conterra' l' indirizzo della console nel quale e' stato creato il task. Questo campo mi servira' per un
	 semplice motivo.Ricordare innanzitutto che se il task viene creato in una determinata console la stampa a video del task deve
	 avvenire in quella determinata console.
	 Quando il task dovra' stampare qualcosa a video verra' controllato se tale campo(detto formalmente, controllera'
	 se la console nel quale e' stato creato il task) e' uguale alla console corrente: se e' cosi' la stampa a video avverra' normalmente,
	 altrimenti(se il task e' stato creato in una console che e' diversa da quella corrente) cio' che il task dovra' stampare a video verra'
	 messo nel buffer temporaneo della console nel quale e' stato creato il task, e quando si riswitchera' a tale console, si
	 stamparenno a video i caratteri memorizzati nel buffer (per maggiori info si veda console.c e console.h).  */
	   
	
	
	//unsigned char owner[MAX_LEN];	//per implementazioni future
	//unsigned char priority;			//per implementazioni future
} task_t;


pid_t create_task(void (*func)(), char *name, unsigned char privilege); //restituisce il pid del processo creato
void end_task(void);
void del_tss(tss_t* tss);
void idle_task(void);
int get_current_pid_task(void);

extern pid_t free_pid;  //sara' sempre uguale al prossimo pid disponibile
extern pid_t pid_cur_task; //sara' sempre uguale al pid del processo corrente
extern task_t process_table[MAX_TASK]; //e' la tabella dei processi

#endif

