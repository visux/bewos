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
* Descrizione: Definizioni di funzioni per la gestione degli IRQ(Interrupt ReQuest)
***********************************************************************************************/



#include<irq.h>
#include<io_port.h>
#include<scheduler.h>

/********************************************************************************
********************************************************************************/
 //Questa funzione inserisce gli indirizzi delle funzioni di gestione(handler) degli interrupt all' interno di un vettore. La posizione e' lo stesso
 //dell' indentificativo dell' interrupt. Se l' interrupt e' 0(timer) allora all' elemento 0 deve esserci l' indirizzo della funzione che gestisce tale
 //interrupt(che gestisce il timer) ecc...
void insert_irq(void (*func)(), int pos)
{
    handler_vector_function[pos] = func;
}
 /********************************************************************************
********************************************************************************/ 
//Questa funzione si occupa di stabilire quale interrupt e' stato ricevuto leggendo il cosidetto registro ISR alla porta 0x0B sia per il PIC
//MASTER(porta 0x20), sia per il PIC SLAVE(porta 0xA0). Se il valore letto e' 0000010 vuol dire che e' stato generato l' interrupt
//1(tastiera), se il valore e' 00000001 vuol dire che e' stato generato l' interrupt 0(timer) ecc... Dunque l' interrupt che e' stato ricevuto viene
//posto a 1 all' interno del registro ISR
void gestisci_irq(unsigned int esp)
{
    char interrupt_request, onebit;
    int position, j;

    position=0; //position tiene conto di quale funzione richiamare all' interno del vettore handler_vector_function
    outportb(0x0B, 0x20);      //Diciamo al PIC MASTER(0x20) che vogliamo leggere il valore del registro ISR(0x0B)
    interrupt_request = inportb(0x20);   //e lo leggiamo    
    if (interrupt_request==0) //se non e' stato trovato alcun interrupt ricevuto nel PIC MASTER
    {
    //Allora leggiamo il registro ISR del PIC SLAVE(0xA0)
        outportb(0x0B, 0xA0);
        interrupt_request = inportb(0xA0);
        position=8; //Poiche gli interrupt da 8 a 16 sono del PIC SLAVE inizializziamo la variabile position a 8
    }
    
   //una volta letto il registro ISR e assegnato il valore a interrupt_request, troviamo la posizione del primo bit settato a 1
    for(j=0; j<8; j++)
	{
           onebit = (interrupt_request & (1 << j));
           if(onebit != 0)
              break;
	}

   position += j; //Aggiungiamo al valore di position il valore di j. Se e' stato ricevuto l' interrupt 1(tastiera) position varra' 0 + 1 = 1
   			   //Questo valore verra' poi di seguito utilizzato per chiamare l' handler specifico dell' interrupt.

   if (!handler_vector_function[position]) return;

   handler_vector_function[position](); //Chiama il giusto handler in base a quale interrupt e' stato ricevuto
   
  //Una volta chiamata la funzione di gestione dell 'interrupt ricevuto bisogna dare un comando di EOI(End Of Interrupt) 
    if (position >= 8)
        outportb(0x20, 0xA0); //End Of Interrupt del PIC SLAVE
	
    outportb(0x20, 0x20); //End Of Interrupt del PIC MASTER
    
 scheduler(&esp);   //chiamo lo scheduler passandoci il valore corrente del registro esp( stack pointer)
 
}
