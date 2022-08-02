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
 
/******************************************INFO*******************************
* Coders: Black
* Descrizione: Definizioni di funzioni per l' inizializzazione del PIT
* (Programmable Interrupt Timer) - chip 8253
******************************************************************************/

#include <pit8253.h>
#include <pic8259.h>
#include <io_port.h>
#include <timer.h>
#include <system.h>
#include <string.h>

/* 
 * Il PIT 8253 ha 3 canali interni detti: timer 0, timer 1 e timer 2. Il primo
 * viene usato come Timer Clock, il secondo per il refresh della RAM e il terzo
 * per emettere suoni attraverso lo speaker. Tutti e tre i canali hanno un
 * oscillatore associato che lavora ad una frequenza base di 1,193182 Mhz.
 * E' possibile programmare ciascun canale, rispettivamente, attraverso le
 * porte di I/O: 0x40, 0x41 e 0x42. Di seguito si utilizza il timer 0  per fare
 * in modo che dal canale 0 fuoriesca un segnale di interrupt sulla linea IRQ 0
 * (associata al timer clock) ad una frequenza di 100Hz. Il PIT utilizza
 * infine un ultimo registro (Mode Control Register) associato alla porta 0x43
 * il cui formato e' il seguente:
 * - bit 6-7 : timer channel
 * - bit 4-5 : LSB (11) o MSB
 * - bit 3-1 : counter mode (011=square wave rate generator)
 * - bit 0 : count format (0 = binary)
 */

/* La funzione che segue configura e inizializza il PIT8253 */
void Init_PIT8253(void)
{
	unsigned long flags;

	/* Salvo gli eflags nella variabile flags e disattivo gli interrupt */	
	save_flags_irq(flags);
    
	/*
	 * Inviamo  il valore 0x36 identificato dalla define PIT_CONTROL_DATA
	 * sulla porta 0x43(Mode Control Register) identificato da PIT_CONTROL.
	 * 0x36 in binario e' 00110110.  I bits 6-7 del mode control register
	 * sono stati impostati a 0 per selezionare il canale 0. I bits 4-5
	 * indicano la modalita' LSB (cioe', quando si inviano delle word sulle
	 * porte 0x40, 0x41 e 0x42, invieremo prima il byte meno significativo
	 * e poi quello piu' significativo). Attraverso i bits 1-3 si informa
	 * il PIT il tipo di timing che si desidera.
	 */
	outportb(PIT_CONTROL_DATA, PIT_CONTROL); 
   
	/* Di seguito inviamo due bytes sulla porta 0x40 dicendo al PIT di
	 * generare un segnale ad onda quandra ad una frequenza di 100 Hz
	 * (ossia ogni 10 ms). Questo segnale arrivera' al PIC attraverso la
	 * linea IRQ 0 (timer) */
	outportb((unsigned char)TIMER_COUNT, PIT_CHANNEL_0);
	outportb((unsigned char)(TIMER_COUNT>>8), PIT_CHANNEL_0);

	/*
	 * Abilito l'irq 0, assegnato al timer e gli assegno l'handler che si
	 * occuperà di gestire l'evento: isr_timer.
	 */
	insert_irq(isr_timer, 0);
	abilita_irq(0, 0x21);
	
	/* Ripristino gli eflags */
	restore_flags(flags);
}

