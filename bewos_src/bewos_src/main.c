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
* Descrizione: File principale nella quale avviene l' inizializzazione del sistema operativo
***********************************************************************************************/


#include<console.h>
#include<io.h>
#include<gdt.h>
#include<pic8259.h>
#include<pit8253.h>
#include<keyboard.h>
#include<shell.h>
#include<string.h>
#include<math.h>
#include<phisicmem.h>
#include<irq.h>
#include<scheduler.h>
#include<task.h>
#include<semaphores.h>
#include<paging.h>
#include<malloc.h>
#include<syscall.h>
#include<time.h>
#include<vfloppy.h>


#define LAST_COMMIT "21/10/2006" //Data dell' ultimo Commit effettuato al CVS
#define BEWOS_VERSION "0.2" //Versione attuale di BeWos

struct  cur_paging cur_info_paging;

pid_t pid_cur_task; //sara' sempre uguale al pid del processo corrente
int a;
sem_t* mutex;

int main()
{

unsigned int k;

pid_cur_task = 0;
init_sys_call();	//inizializzo le system calls
init_consoles(); //inizializzo le consoles video
asm("cli");


/*printk("\n\n");
printk("\t  oooooooooo             oooo     oooo                       \n");
printk("\t   888    888  ooooooooo8 88   88  88  ooooooo    oooooooo8 \n");
printk("\t   888oooo88  888oooooo8   88 888 88 888     888 888ooooooo  \n");
printk("\t   888    888 888           888 888  888     888         888 \n");
printk("\t  o888ooo888   88oooo888     8   8     88ooo88   88oooooo88  \n");
printk("\n\n");*/
//printk("\n\n");

set_text_color(DEFAULT_TEXT_COLOR);
printk("               \t\t  __            \n");
printk("Welcome to:    \t\t |__) _|  | _  _ \n");
set_text_color(WHITE_COLOR + BRIGHT_COLOR);
printk("Da snapshot CVS del: %s", LAST_COMMIT);
set_text_color(DEFAULT_TEXT_COLOR);
printk(" |__)(-|/\\|(_)_)    %s      www.bewos.org\n", BEWOS_VERSION);
for(k=0;k<SCR_C;k++) printk(".");

set_text_color(CYAN_COLOR);
size_mem(); //Calcolo della ram in byte. Il valore verra' messo nella variabile esterna total_mem
printk("Dimensione RAM:  %d Mb", total_mem/(1024*1024)); //Stampo il valore in MB
printok();

init_pages();
printk("Inizializzazione delle pagine fisiche da %d bytes...", SIZE_PAGE);
printok();
set_text_color(MAGENTA_COLOR);
printk("  - Numero di pagine fisiche disponibili: %d\n", max_pages_intomem);
printk("  - Numero degli elementi della bitmap: %d\n", real_elem_vett);
set_text_color(CYAN_COLOR);


printk("Inizializzazione della Paginazione hardware (pagine di %d bytes)...",SIZE_PAGE);
init_paging();
printok();


printk("Inizializzazione della GDT - Global Descriptor Table...");
initGDT();
printok();


vfloppy_init();
printk("Disco virtuale (1.44 MB: 2880 settori,512 bytes per settore%s)",(vfloppy_flags & VFLOPPY_READONLY)?",Sola lettura":""); // Stampo il puntatore alla memoria di vfloppy
printok();


printk("Inizializzazione del PIC8259...");
Init_PIC8259();
printok();

printk("Inizializzazione dell' IDT - Interrupt Descriptor Table...");
initIDT();
printok();

//setup_timer();
printk("Inizializzazione del PIT8253...");
Init_PIT8253();
printok();


printk("Inizializzazione del MultiTasking...");
printok();
set_text_color(MAGENTA_COLOR);
printk("  - Task Idle creato con pid 0");
init_scheduler();
set_text_color(CYAN_COLOR);

// La funzione precedente non porta a capo dopo avere scritto "task idle creato con pid 0" quindi porto a capo prima della nuova scritta
printk("\nInizializzazione della Keyboard...");
initKBD();
printok();

set_text_color(DEFAULT_TEXT_COLOR);
for(k=0;k<SCR_C;k++) printk(".");

set_text_color(WHITE_COLOR + BRIGHT_COLOR);
printk("\t\t\tSistema inizializzato ! ");
set_text_color(DEFAULT_TEXT_COLOR);

printk("\n\n");

init_console=1; /* La console che sto creando è quella iniziale
La funzione shell come prima cosa controlla questa variabile. Se è settata
lascia quello che era stato già scritto, cioè, in questo caso le informazioni
di avvio stampate da main.c

Le successive consoles, droveranno questa variabile non settata e
cancelleranno lo schermo, in modo da non lasciare quello che era stato
già scritto da altre shell della stessa console killate
*/

asm("sti");


cur_console->pid_task_shell = create_task(shell,"shell", USER_PRIVILEGE);

	while(1);
	return 0;
}
