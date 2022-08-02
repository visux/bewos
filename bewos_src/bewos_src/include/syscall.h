#ifndef __SYSCALL_H
#define __SYSCALL_H

#include<task.h>

/* ****	Come funzionano le syscall	****
Le syscall sono particolari funzioni che "lavorano" a livello kernel(ring 0). Un task in user mode puo' chiamare una syscall in modo che quest' ultima possa interagire direttamente con il kernel e possa gestire le richieste user. Per passare da user mode a kernel mode si utilizza un interrupt software(generalmente 0x80).

L' implementazione delle syscall e' " un po' " sistematica. Una syscall e' composta da tre principali "funzioni": una "wrapper rotine", "una syscall handler", che e' uguale per tutte le syscall(che e' associata all' indice 0x80 della IDT) e una funzione di servizio(che puo essere definita come la syscall vera e propria).

Ad ogni syscall e' associato un numero identificativo chiamato "system call number".
L' indirizzo di ogni funzione di servizio(le syscall vere e proprie) e' messo in un elemento di un array(la system call table) avente per indice il "system call number" della syscall. 

Ipotiziamo che vogliamo creare due syscall: sys_prova e sys_try i cui identificativi si trovano nelle define _NR_prova e _NR_try
La system call table verra inizializzata in questa maniera:
system_call_table[_NR_prova] = sys_prova;
system_call_table[_NR_try] = sys_try;

Ovviamente non e' possibile assegnare a piu' di una syscall lo stesso identificativo.


*Le Wrapper routines*

Il compito delle "Wrapper routines" e'

- Inserire nei registri generali i parametri da passare alla syscall
- Inserire nel registro "eax" l' identificativo della system call( il system call number)
- Eseguire l' interrupt 0x80(attraverso l' istruzione "int $0x80")
- Ritornare un valore(negativo,in caso di errore, positivo, se e' tutto andato bene) al task che ha "chiamato" la syscall.



*La system call handler*

La system call handler viene eseguita quando la wrapper routine esegue l' istruzione "int $0x80". All' indice 0x80(128) dell' IDT e' stata infatti associata l' handler che esegue le seguenti operazioni:

- Controlla se nel registro eax c' e' un system call number valido.
- Pusha sullo stack il registro di segmento dati
- Pusha sullo stack il registro es
- Pusha sullo stack gli argomenti della syscall. Ricordo che gli argomenti della syscall sono stati messi nei registri generali dalla wrapper routines, quindi bisogna pushare tali registri
- Aggiorna i registri di segmento con il descrittore kernel data
- Esegue la system call attraverso l' istruzione "call	*system_call_table(,%%eax,4) \n". 
In pseudo codice equivale a: esegui systemcall_table[eax]. Ricordo che in eax si trova il system call number
- Una volta terminata la syscall ripristina i registri
- Esce dall' interrupt con l' istruzione "iret"



*Funzioni di servizio(sycall vere e proprie)

Tali funzioni che lavorano in kernel mode possono a questo punto comunicare con il kernel, gestire le periferiche, eseguire istruzioni privilegiate ecc...


Prendiamo come esempio la system call "debug".
La wrapper routine viene definita attraverso le macro sottostanti ed essa puo essere chiamata dai task semplicemente facendo: 
debug(par1, par2);

La funzione di routine e' cosi' dichiarata: int sys_debug(int number, int number2);
Tale funzione non fa altro che stampare i parametri a video.

Per eseguire la syscall basta chiamare la wrapper routine, quindi: debug(par1, par2);
par1 e par2 sono due ipotetiche variabili di tipo "int". Una volta entrati nella wrapper routine questa mettera' nei registri generali i due parametri e il system call number nel registro eax ed eseguira' l' interrupt 0x80 attraverso l' istruzione "int $0x80". A questo punto si entrera' nell' handler associato all' indice 0x80(128) della IDT. L' handler, dopo aver pushato i parametri sullo stack(tali parametri si trovano nei registri generali e sono stati messi dalla wrapper routine), chiama la funzione di servizio sys_debug attraverso l' istruzione "call	*system_call_table(,%%eax,4)". una volta eseguita questa istruzione ci troviamo in sys_debug la cui definizione e' : 
int sys_debug(int number, int number2) { ....... } . Automaticamente i parametri che erano stati pushati sullo stack verranno messi in number e in number2 e verranno successivamente stampati. Quando questa funzione ritornera' , cosi' come l' handler e la wrapper routine ci si ritrovera' di nuovo nel task di partenza che continuera' ad eseguire il resto delle istruzioni.    */





/* Costanti che associano ad ogni syscall un numero identificativo */
#define __NR_debug		0
#define __NR_getpid		1
#define __NR_sleep		2
#define __NR_getche  		3
#define __NR_puts		4
#define __NR_fork		5
#define __NR_kill_process	6
#define __NR_vfloppy_raw_read	7
#define __NR_vfloppy_raw_write	8
#define __NR_do_timer		9
#define __NR_do_sem		10
/*---------------------------------------------------------------*/



/* La costante indica il massimo numero di syscall possibili. */
#define NR_syscalls 200


extern int errno;

//Una syscall ritorna sempre un valore intero. Esso e' negativo se la syscall non e' andata a buon fine; positivo se e' andata a buon fine
#define __syscall_return(type, res) \
do { \
	if ((unsigned long)(res) >= (unsigned long)(-125)) { \
		errno = -(res); \
		res = -1; \
	} \
	return (type) (res); \
} while (0)


/* Le macro che seguono DEFINISCONO le wrapper routines. 


Una wrapper routine, imposta opportunamente alcuni parametri come l' id che il kernel ha associato a quella determinata system call(ad esempio _NR_debug) e invoca un software interrupt (0x80 nel nostro caso). Questo provoca lo switch della CPU in kernel mode con la conseguente esecuzione di una routine uguale per qualsiasi system call invocata(ovverro la routine system_call_handler  presente all' indice 128(0x80) dell' IDT)

Ad ogni system call, dunque, �associata una wrapper routine definita attraverso una delle macro sottostanti _syscallx, con x che pu�assumere un valore da 0 a 5. Questo valore indica il numero di parametri che accetta la routine. I primi due parametri di questa macro sono il tipo di ritorno della system call e il suo nome. Successive coppie di parametri indicano il tipo e nome dei parametri della system call. 	

Visto il numero esiguo di registri disponibili su architetture Intel non pi di 6 parametri �possibile passare alle system calls, in particolare, si utilizza il registro eax per contenere l' id della system call e ebx, ecx, edx, esi e edi per gli altri parametri.. Se si rende necessario passare pi di 6 parametri, si pu�sempre usare un buffer, contenente uno o pi parametri, puntato da uno dei registri.


*/

/* Definisce una wrapper routine senza parametri in ingresso */

#define _syscall0(type,name) \
type name(void) \
{ \
long __res; \
__asm__ volatile ("int $0x80" \
	: "=a" (__res) \
	: "0" (__NR_##name)); \
__syscall_return(type,__res); \
}



/* Definisce una wrapper routine con *un* parametro in ingresso */

#define _syscall1(type,name,type1,arg1) \
type name(type1 arg1) \
{ \
long __res; \
__asm__ volatile ("int $0x80" \
	: "=a" (__res) \
	: "0" (__NR_##name),"b" ((long)(arg1))); \
__syscall_return(type,__res); \
}


/* Definisce una wrapper routine con *due* parametri in ingresso */

#define _syscall2(type,name,type1,arg1,type2,arg2) \
type name(type1 arg1,type2 arg2) \
{ \
long __res; \
__asm__ volatile ("int $0x80" \
	: "=a" (__res) \
	: "0" (__NR_##name),"b" ((long)(arg1)),"c" ((long)(arg2))); \
__syscall_return(type,__res); \
}


/* Definisce una wrapper routine con *tre* parametri in ingresso */

#define _syscall3(type,name,type1,arg1,type2,arg2,type3,arg3) \
type name(type1 arg1,type2 arg2,type3 arg3) \
{ \
long __res; \
__asm__ volatile ("int $0x80" \
	: "=a" (__res) \
	: "0" (__NR_##name),"b" ((long)(arg1)),"c" ((long)(arg2)), \
		  "d" ((long)(arg3))); \
__syscall_return(type,__res); \
}


/* Definisce una wrapper routine con *quattro* parametri in ingresso */

#define _syscall4(type,name,type1,arg1,type2,arg2,type3,arg3,type4,arg4) \
type name (type1 arg1, type2 arg2, type3 arg3, type4 arg4) \
{ \
long __res; \
__asm__ volatile ("int $0x80" \
	: "=a" (__res) \
	: "0" (__NR_##name),"b" ((long)(arg1)),"c" ((long)(arg2)), \
	  "d" ((long)(arg3)),"S" ((long)(arg4))); \
__syscall_return(type,__res); \
} 


/* Definisce una wrapper routine con *cinque* parametri in ingresso */

#define _syscall5(type,name,type1,arg1,type2,arg2,type3,arg3,type4,arg4, \
	  type5,arg5) \
type name (type1 arg1,type2 arg2,type3 arg3,type4 arg4,type5 arg5) \
{ \
long __res; \
__asm__ volatile ("int $0x80" \
	: "=a" (__res) \
	: "0" (__NR_##name),"b" ((long)(arg1)),"c" ((long)(arg2)), \
	  "d" ((long)(arg3)),"S" ((long)(arg4)),"D" ((long)(arg5))); \
__syscall_return(type,__res); \
}



/* Nel codice soprastante(prendiamo come esempio _systemcall5), i 5 argomenti per la syscall sono messi in %ebx, %ecx, %edx, %esi e %edi usando i "constraints"(cosi' chiamati) b, c, d, S e D. Si nota che  "=a" e' usato per l' output cosi' che il valore di ritorno della syscall, situato in %eax, sia messo nella variabile __res. Usando il contraint "0"(come primo operando constraint) nella sezione di input, il numero di syscall __NR_##name(che identifica la syscall) e' messo in %eax. Dunque %eax viene utilizzato sia come input che come output senza utilizzare registri separati. Si Nota anche che l' input(il numero di syscall) e' consumato (usato) prima che output (il valore di ritorno della syscall) e' prodotto. Il codice imposta prima i registri ed *infine* esegue "int $0x80".

Alcuni dei "contraint" gcc sono i seguenti:

- "a", "b", "c", "d" - %eax, %ebx, %ecx, %edx
- "S", "D" - %esi,  %edi


Altra cosa da notare e' il tocken "##"  presente in __NR_##name. Il token `##' e' conosciuto come uno speciale "segno" nell' ANSI C.
il suo uso e' molto intuitivo. Nel caso del codice soprastante se alla macro verra' passato come secondo argomento "ciao" allora "__NR_##name"  sara'  "espanso" come __NR_ciao. Se invece al parametro "name"(che e' il secondo argomento della macro) verra' passato "hello" allora sara'  "espanso" come __NR_hello e cosi' via. 

Le macro soprastanti sono molto utili per non riscrivere le definizioni delle wrapper routines per ogni syscall. Infatti attraverso tali macro e' possibile definire tutte le wrapper routines delle syscall(o almeno tutte quelle che non richiedono piu' di 5 parametri).

Facciamo un esempio:

vogliamo creare una syscall di nome "prova" secondo tale dichiarazione: int prova(int arg); 
poi definiamo il suo numero di syscall __NR_prova 1

poi definiamo la wrapper rotine facendo semplicemente: static inline syscall1(int, prova, int, arg) 
utilizziamo la macro "_syscall1" perche' la syscall riceve solo un parametro di ingresso ovvero "int arg"

L' operazione verra' ripetuta per tutte le altre syscall che si vogliono creare

*/

void* system_call_table[NR_syscalls]; //Dichiaro la tabella delle systemcall

extern void system_call_handler(void); //dichiaro l' handler per le syscall
void setup_syscall_table(void* func, unsigned int syscall_number);
void init_sys_call(void); //Dichiaro la funzione che inizializza le syscall
void syscall_not_found(void);  //Nel caso in cui viene specificato un system call number non valido viene chiamata questa funzione

#include <vfloppy.h>	/* Prototipi di vfloppy_raw_* */
#include <timer.h>	/* Prototipo per sys_do_timer */
#include <semaphores.h>	/* Prototipo per sys_do_sem */

//prototipi delle wrapper routines
static __inline__ _syscall2(int, debug, int, number, int, number2);
static __inline__ _syscall0(int, getpid);
static __inline__ _syscall1(int, sleep, unsigned long int, ms);
static __inline__ _syscall0(int, getche);
static __inline__ _syscall1(int, puts, const char*, s);
static __inline__ _syscall3(int, fork, void*,  func,  char*, name, unsigned char, privilege);
static __inline__ _syscall1(void, kill_process, pid_t, pidnum);
static __inline__ _syscall2(int, vfloppy_raw_read, int, blocknum, void*, buffer);
static __inline__ _syscall2(int, vfloppy_raw_write, int, blocknum, void*, buffer);
static __inline__ _syscall5(struct timer *, do_timer, int, op, struct timer *, timer,
			    unsigned long, ms, void *, handler, void *, data);
static __inline__ _syscall2(void, do_sem, int, op, sem_t *, semaphore);

//prototipi delle funzioni di servizio(le syscall vere e proprie)
int sys_debug(int number, int number2);
int sys_getpid(void);
int sys_sleep(unsigned long int ms);
int sys_getche(void);
int sys_puts(const char* s);
int sys_fork(void (*func)(), char* name, unsigned char privilege);
void sys_kill_process(pid_t pidnum);

#endif
