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
* Descrizione: Definizioni di funzioni per l' inizializzazione dell' *I*nterrupt *D*escriptor *T*able
*******************************************************************************************************/



#include<idt.h>
#include<irq.h>
#include<io.h>
#include<io_port.h>
#include<gdt.h>
#include<syscall.h>
#include<system.h> //Per le macro save_flags e restore_flags


typedef struct descrittore vettore_di_interrupt;
vettore_di_interrupt IDT[DIM_IDT];

void system_call_handler(void);

/********************************************************************************
********************************************************************************/
//handler di default
void handler_default(void)
{
  printk("\n\n Handler di default\n");
  asm("iret"); //Ritorna dall' interrupt
}
/********************************************************************************
********************************************************************************/
/*Funzione per impostare l'idtr. Ricordo che il registro idtr deve contenere un dato di 48 bit. 16 bit che 
  indicano la dimensione dell' IDT e 32 bit che indica l' indirizzo base dell' IDT. */
void carica_idt(vettore_di_interrupt* base_addr, int num_desc) {
		unsigned int idt_reg[2];
		idt_reg[0]=(num_desc*8) << 16;
		idt_reg[1]=(unsigned int)base_addr;

	           asm("": : :"memory");
		__asm__ __volatile__ ("lidt (%0)": :"g" ((char *)idt_reg+2));

	}
/********************************************************************************
********************************************************************************/
//Tramite questa funzione assegniamo ad ogni interrupt il proprio handler(ovvero la propria funzione di gestione)
void Init_IDT_entry(void (*handler)(void), int indice, unsigned short opz){
        if(indice < DIM_IDT){   /* Vediamo se si tratta di un interrupt valido. Se è minore della dimensione 
				  dell' IDT allora è valido */

                IDT[indice].offset0_15 = (unsigned int)handler & 0xFFFF;       //Consideriamo i bit da 0 a 15
		IDT[indice].offset16_31 = (unsigned int)handler >> 16; //Consideriamo i bit da 16 a 31
		IDT[indice].segmento = 0x8;      //Segmento codice
		IDT[indice].opzioni = opz;   //Opzioni del descrittore
	}

else
 printk("\n\nInterrupt non valido\n");
}
/********************************************************************************
********************************************************************************/
//Inizializzazione dell' IDT
void initIDT(void){
int i, j;
long flags;

		save_flags(flags); //salvo gli Eflags nella variabile flags attraverso la macro save_flags
		asm("cli"); //Disabilito gli interrupt

	//impostiamo L' IDT con gli handler di default
         for(i = 17; i < DIM_IDT; i++)
                Init_IDT_entry(handler_default, i, 0x470);

	/* Gestiamo le Eccezioni. Opzioni: Presente;  privilegio: Supervisor*/

	Init_IDT_entry(trap0, 0, 0x470);
	Init_IDT_entry(trap1, 1, 0x470);
	Init_IDT_entry(trap2, 2, 0x470);
	Init_IDT_entry(trap3, 3, 0x470);
	Init_IDT_entry(trap4, 4, 0x470);
	Init_IDT_entry(trap5, 5, 0x470);
	Init_IDT_entry(trap6, 6, 0x470);
	Init_IDT_entry(trap7, 7, 0x470);
	Init_IDT_entry(trap8, 8, 0x470);
	Init_IDT_entry(trap9, 9, 0x470);
	Init_IDT_entry(trap10, 10, 0x470);
	Init_IDT_entry(trap11, 11, 0x470);
	Init_IDT_entry(trap12, 12, 0x470);
	Init_IDT_entry(trap13, 13, 0x470);
	Init_IDT_entry(trap14, 14, 0x470);
	Init_IDT_entry(trap15, 15, 0x470);
	Init_IDT_entry(trap16, 16, 0x470);
	
	//all'indice 128(0x80) metto l' handler per le system call. Opzioni: Presente;   privilegio: User
	Init_IDT_entry(system_call_handler, 128, 0x770); 

  /* Fine della gestione delle eccezioni */

  /* Inizializzo gli interrupt che hanno un indice che va da 0x20(32 in decimale) a 0x2F(47 in decimale) con l'handler in asm. Questi
  interrupt rappresentano il timer(0x20), la tastiera(0x21) ecc... Quando verra' premuto un tasto, ad esempio, verra' chiamato l'handler in
  asm che a sua volta chiamara' la funzione che gestisce gli irq.  */
  	     for(j=32; j<48; j++)
	      	{
		if(j == 128) //l' indice 128 l' ho gia' inizializzato con l' handler "system_call_handler");
		  continue;
		else
      		  Init_IDT_entry(handler_asm, j, 0x470);
  		}
  
	/* A questo punto possiamo chiamare la funzione che si occupa di caricare l' indirizzo dell' IDT 
	   nel registro idtr */
	carica_idt(IDT, DIM_IDT);
	

		restore_flags(flags);  //Ripristino gli Eflags
}



//Handler delle eccezioni

void trap0(void)
{
printk("\n\n #Trap0#: Division By Zero\n");
while(1);
}


void trap1(void)
{
printk("\n\n #Trap1#: Trap of Debug\n");
  while(1);
}


void trap2(void)
{
printk("\n\n #Trap2#: NMI !!!\n");
  while(1);
}


void trap3(void)
{
printk("\n\n #Trap3#: Breakpoint\n");
  while(1);
}


void trap4(void)
{
printk("\n\n #Trap4#: Overflow\n");
  while(1);
}


void trap5(void)
{
printk("\n\n #Trap5#: Bound Range Exceeded\n");
  while(1);
}


void trap6(void)
{
printk("\n\n #Trap6#: Invalid Opcode\n");
  while(1);
}


void trap7(void)
{
printk("\n\n #Trap7#: Coprocessor Not Available\n");
  while(1);
}


void trap8(void)
{
printk("\n\n #Trap8#: Double Fault\n");
  while(1);
}


void trap9(void)
{
printk("\n\n #Trap9#: Coprocessor Segment Overrun\n");
  while(1);
}


void trap10(void)
{
printk("\n\n #Trap10#: Invalid Task State Segment\n");
  while(1);
}


void trap11(void)
{
printk("\n\n #Trap11#: Segment Not Present\n");
  while(1);
}


void trap12(void)
{
printk("\n\n #Trap12#: Stack Fault\n");
  while(1);
}


void trap13(void)
{
printk("\n\n #Trap13#: General Protection\n");
  while(1);
}


void trap14(void)
{
printk("\n\n #Trap14#: Page Fault\n");
  while(1);
}


void trap15(void)
{
printk("\n\n #Trap15#: UNKNOWN ?????\n");
  while(1);
}


void trap16(void)
{
printk("\n\n #Trap16#: Error of Floating Point\n");
  while(1);
}


void trap17(void)
{
printk("\n\n #Trap17#: Memory Not Alligned\n");
  while(1);
}
//Fine handler delle eccezioni
/********************************************************************************
********************************************************************************/
/* Quando c'è bisogno di gestire un irq(ad esempio quando si preme un tasto) verrà chiamata questa funzione, la quale non può
   essere una funzione C visto che il compilatore mette RET come istruzione finale di una funzione, mentre un interrupt handler
   deve terminare con IRET. Ricordo che quando avviene un interrupt(ad esempio quello del timer) il processore salva
   AUTOMATICAMENTE sullo stack corrente i valori dei registro eflags, cs ed eip che vengono ripristinati con la iret.
   Se vi e' un cambio di privilegio (ring3->ring0) il processore pusha anche ss e esp corrente. Quindi pushera' ss, esp, eflags, cs, eip
     */

    
void handler_asm()
 {

    asm volatile (".globl handler_asm  \n"

    /* Disabilito gli interrupt con 'cli' e salvo i registri generali */

         "handler_asm: \n"
         "cli \n"
	 
         "pusha \n"
	 
	//Salvo i registri Segmento sullo stack
	 "pushl   %ds \n"
	 "pushl   %es \n"
	 "pushl   %fs \n"
	 "pushl   %gs \n"
	 
    /*salvo esp nel registro eax */ 	 
	 "movl %esp, %eax  \n"
   /* pusho il valore di esp contenuto nel registro eax */	  
         "pushl %eax \n"  

    /* Quindi chiamo la funzione scritta in C che si occupa di gestire gli irq. Poiche' il valore di esp e' stato pushato sullo stack
       alla funzione gestisci_irq verra' passato proprio questo valore come parametro. Ricordo che gestisci_irq chiama ad ogni
       interrupt lo scheduler che se e' abilitato(si abilita ogni 10 interrupt timer) mette sullo stack il valore dello stack del task che
        andra' in esecuzione. */
         "call gestisci_irq \n"

    
	 "popl %eax  \n" /*mi prendo il valore di esp dallo stack e lo assegno al registro eax. Questo valore puo' anche non
	  corrispondere all' esp pushato sullo stack precedentemente(prima della chiamata a "gestisci_irq"). Infatti lo
	  scheduler(chiamato dalla funzione "gestisci_irq") puo' aver messo sullo stack un nuovo valore. Si cambia task e si cambia
	   anche stack! Ovviamente consiglio di vedere i file scheduler.c e irq.c*/
         "mov %eax, %esp  \n" /* metto il valore di eax nel registro esp(reimposto il puntatore allo stack) */

	  //Ripristino i registri Segmento dallo stack
	 "popl %gs \n"
	 "popl %fs \n"
	 "popl %es  \n" 
	 "popl %ds \n"
	 

         "popa \n" /* ripristino tutti i registri generali */ 
	 
	 
         "sti \n"  /*riabilito gli interrupt*/ 

	"iret \n" //Ritorna dall' interrupt
	/*una iret AUTOMATICAMENTE effettua il pop(ripristino) dei valori dei registri eip, cs e eflags nei corrispettivi registri. Se
	 cs:eip fanno riferimento ad un codice operante in usermode allora la iret ripristina anche ss ed esp dallo stack corrente*/
	
 
 );

 }
 
