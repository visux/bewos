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
* Descrizione: Definizioni di funzioni per l' inizializzazione del PIC - chip 8259
***********************************************************************************************/



#include<pic8259.h>
#include<io_port.h>
#include<system.h> //Per le macro save_flags e restore_flags


/*Questa che segue è un array che serve per abilitare il giusto irq. Ricordo che se vogliamo abilitare l' irq
 0 (timer) dobbiamo inviare alla giusta porta (0x21 per master e 0xA1 per slave) 11111110 ovvero
 l' irq da attivare deve essere posto a 0. Se vogliamo abilitare il 1° irq (tastiera) allora inviamo 
 11111101 e così via. */

unsigned char irq_number[8] = {
			    0xfe, 	//11111110 - Master PIC: Timer     --  Slave PIC: Real Time Clock
			    0xfd,  	//11111101 - Master PIC: Tastiera  --  Slave PIC: Free
			    0xfb,	//11111011 - Master PIC: Slave PIC --  Slave PIC: Free
			    0xf7,	//11110111 - Master PIC: COM2/COM4 --  Slave PIC: Free
			    0xef,	//11101111 - Master PIC: COM1/COM3 --  Slave PIC: Free
			    0xdf,	//11011111 - Master PIC: LPT2      --  Slave PIC: Coprocessore
			    0xbf,	//10111111 - Master PIC: Floppy    --  Slave PIC: 1° hardisk
			    0x7f	//01111111 - Master PIC: LPT1      --  Slave PIC: 2° hardisk
			    };

/********************************************************************************
********************************************************************************/
// Inizializzo il PIC - Con il pic si comunica tramite le porte 0x20 - 0x21 per il master e 0xA0 - 0xA1 per il slave
void Init_PIC8259(void)
{

int i;
long flags;
static const unsigned irq0_int = 0x20, irq8_int = 0x28;


		save_flags(flags); //salvo gli Eflags nella variabile flags attraverso la macro save_flags
		asm("cli"); //Disabilito gli interrupt

/* Initialization Control Word #1 (ICW1) */
	outportb(0x11, 0x20);
	outportb(0x11, 0xA0);

/* Initialization Control Word #2 (ICW2) */

//route IRQs 0-7 to INTs 20h-27h
	outportb(irq0_int, 0x21); //Con questo comando facciamo in modo che all' irq0 corrisponda il vettore di interruzione 0x20, all' irq1 0x21 ecc.

// route IRQs 8-15 to INTs 28h-2Fh
	outportb(irq8_int, 0xA1); //Idem ma per lo slave

/* Initialization Control Word #3 (ICW3) */
	outportb(0x04, 0x21);
	outportb(0x02, 0xA1);

/* Initialization Control Word #4 (ICW4) */
	outportb(0x01, 0x21);
	outportb(0x01, 0xA1);
	
	
//Disabilito le linee irq

//Disabilito gli interrupt per il PIC MASTER
for(i=0; i <= 7; i++)
disabilita_irq(i, 0x21);
	
//Disabilito gli interrupt per il PIC SLAVE
for(i=0; i <= 7; i++)
disabilita_irq(i, 0xA1);

		restore_flags(flags);  //Ripristino gli Eflags

}
/********************************************************************************
********************************************************************************/
void abilita_irq(unsigned char irq_num, unsigned char port_num)
{
    char dato;
    dato = inportb(port_num); //leggiamo per dapprima il valore della porta port_num
    
    dato = dato & irq_number[irq_num]; //qui facciamo la mask dell' irq da attivare con il valore gia' presente sulla porta port_num
    									//Se vogliamo attivare l' irq 0 ma e' gia attivo l' irq 1 si avra' una situazione del genere
									//dato e' uguale a 11111110 
									//irq_number e' uguale a 11111101
									//Facendo  dato & irq_number[irq_num]; si avra':
									//11111110 
									//     &
									//11111101 
									//e si otterra' 
									//11111100
									//In questa maniera saranno abilitati entrambi gli irq !

    outportb(dato, port_num);  // Invia a portnum (0x21 o 0xA1) la mask dell' irq da attivare.

}


void disabilita_irq(unsigned char irq_num, unsigned char port_num)
{
    char dato;
    dato = inportb(port_num); //leggiamo per dapprima il valore della porta port_num
    
    dato = dato | (1<<irq_num ); //a dato verra assegnato il valore per cui il bit irq_num varra' 1

    outportb(dato, port_num);  // Invia a portnum (0x21 o 0xA1) la mask dell' irq da disattivare.

}
